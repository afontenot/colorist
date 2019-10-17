// ---------------------------------------------------------------------------
//                         Copyright Joe Drago 2018.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "colorist/context.h"

#include "colorist/version.h"

#include "lcms2.h"

#include "avif/avif.h"
#include "jpeglib.h"
#include "openjpeg.h"
#include "png.h"
#include "zlib.h"

// WebP
#include "decode.h"
#include "encode.h"

#include <stdio.h>

#if !defined(GIT_COMMIT)
#define GIT_COMMIT "Unknown"
#endif

void clContextPrintVersions(clContext * C)
{
    char avifCodecs[256];
    avifCodecVersions(avifCodecs);

    int version;
    clContextLog(C, NULL, 0, "Versions:");
    clContextLog(C, NULL, 1, "colorist   : %s", COLORIST_VERSION_STRING);
    clContextLog(C, NULL, 1, "avif       : %s (%s)", avifVersion(), avifCodecs);
#if defined(COLORIST_LIBJPEG_TURBO)
    clContextLog(C, NULL, 1, "jpeglib    : %d (turbo)", JPEG_LIB_VERSION);
#else
    clContextLog(C, NULL, 1, "jpeglib    : %d", JPEG_LIB_VERSION);
#endif
    clContextLog(C, NULL, 1, "lcms2      : %d.%d", LCMS_VERSION / 1000, (LCMS_VERSION % 1000) / 10);
    clContextLog(C, NULL, 1, "libpng     : %s", PNG_LIBPNG_VER_STRING);
    clContextLog(C, NULL, 1, "openjpeg   : %s", opj_version());
    clContextLog(C, NULL, 1, "zlib       : %s", ZLIB_VERSION);
    version = WebPGetDecoderVersion();
    clContextLog(C, NULL, 1, "WebP Decode: %d.%d.%d", (version >> 16) & 0xFF, (version >> 8) & 0xFF, version & 0xFF);
    version = WebPGetEncoderVersion();
    clContextLog(C, NULL, 1, "WebP Encode: %d.%d.%d", (version >> 16) & 0xFF, (version >> 8) & 0xFF, version & 0xFF);
    clContextLog(C, NULL, 0, "");
#if defined(HOMEBREW_BUILD)
    clContextLog(C, NULL, 0, "Built with Homebrew.");
    clContextLog(C, NULL, 0, "");
#else
    clContextLog(C, NULL, 0, "Git Commit : %s", GIT_COMMIT);
    clContextLog(C, NULL, 0, "");
#endif
}
