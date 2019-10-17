// Copyright 2019 Joe Drago. All rights reserved.
// SPDX-License-Identifier: BSD-2-Clause

#include "avif/avif.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[])
{
    const char * inputFilename = NULL;
    if (argc < 2) {
        return 0;
    }
    inputFilename = argv[1];

    FILE * inputFile = fopen(inputFilename, "rb");
    if (!inputFile) {
        fprintf(stderr, "Cannot open file for read: %s\n", inputFilename);
        return 1;
    }
    fseek(inputFile, 0, SEEK_END);
    size_t inputFileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    if (inputFileSize < 1) {
        fprintf(stderr, "File too small: %s\n", inputFilename);
        fclose(inputFile);
        return 1;
    }

    avifRWData raw = AVIF_DATA_EMPTY;
    avifRWDataRealloc(&raw, inputFileSize);
    if (fread(raw.data, 1, inputFileSize, inputFile) != inputFileSize) {
        fprintf(stderr, "Failed to read %zu bytes: %s\n", inputFileSize, inputFilename);
        fclose(inputFile);
        avifRWDataFree(&raw);
        return 1;
    }

    fclose(inputFile);
    inputFile = NULL;

    avifDecoder * decoder = avifDecoderCreate();
    // avifDecoderSetSource(decoder, AVIF_DECODER_SOURCE_PRIMARY_ITEM);
    avifResult result = avifDecoderParse(decoder, (avifROData *)&raw);
    if (result == AVIF_RESULT_OK) {
        printf("AVIF container reports dimensions: %ux%u (@ %u bpc)\n", decoder->containerWidth, decoder->containerHeight, decoder->containerDepth);
        for (int loop = 0; loop < 2; ++loop) {
            printf("Image decoded: %s\n", inputFilename);
            printf(" * %2.2f seconds, %d images\n", decoder->duration, decoder->imageCount);
            int frameIndex = 0;
            while (avifDecoderNextImage(decoder) == AVIF_RESULT_OK) {
                printf("  * Decoded frame [%d] [pts %2.2f] [duration %2.2f] [keyframe:%s nearest:%u]: %dx%d\n",
                       frameIndex,
                       decoder->imageTiming.pts,
                       decoder->imageTiming.duration,
                       avifDecoderIsKeyframe(decoder, frameIndex) ? "true" : "false",
                       avifDecoderNearestKeyframe(decoder, frameIndex),
                       decoder->image->width,
                       decoder->image->height);
                ++frameIndex;
            }

            if (loop != 1) {
                result = avifDecoderReset(decoder);
                if (result == AVIF_RESULT_OK) {
                    printf("Decoder reset! Decoding one more time.\n");
                } else {
                    printf("ERROR: Failed to reset decode: %s\n", avifResultToString(result));
                    break;
                }
            }
        }
    } else {
        printf("ERROR: Failed to decode image: %s\n", avifResultToString(result));
    }

#if 0
    int frameIndex = 25;
    if (avifDecoderNthImage(decoder, frameIndex) == AVIF_RESULT_OK) {
        printf("  * Decoded frame [%d] [pts %2.2f] [duration %2.2f] [keyframe:%s nearest:%u]: %dx%d\n",
               frameIndex,
               decoder->imageTiming.pts,
               decoder->imageTiming.duration,
               avifDecoderIsKeyframe(decoder, frameIndex) ? "true" : "false",
               avifDecoderNearestKeyframe(decoder, frameIndex),
               decoder->image->width,
               decoder->image->height);
    }
#endif

    avifRWDataFree(&raw);
    avifDecoderDestroy(decoder);
    return 0;
}
