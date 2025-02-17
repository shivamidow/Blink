// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/BitmapPattern.h"

#include "platform/graphics/skia/SkiaUtils.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkShader.h"

namespace blink {

BitmapPattern::BitmapPattern(PassRefPtr<Image> image, RepeatMode repeatMode)
: BitmapPatternBase(repeatMode, 0 )
{
    if (image) {
        // If image is animated, what about the pattern?
        m_tileImage = image->nativeImageForCurrentFrame();

        if (m_tileImage)
            adjustExternalMemoryAllocated(m_tileImage->bitmap().getSafeSize());
    }
}

PassRefPtr<SkShader> BitmapPattern::createShader()
{
    if (!m_tileImage) {
        return adoptRef(SkShader::CreateColorShader(SK_ColorTRANSPARENT));
    }

    SkMatrix localMatrix = affineTransformToSkMatrix(m_patternSpaceTransformation);

    if (isRepeatXY()) {
        return adoptRef(SkShader::CreateBitmapShader(m_tileImage->bitmap(), SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode, &localMatrix));
    }

    return BitmapPatternBase::createShader();
}

SkImageInfo BitmapPattern::getBitmapInfo()
{
    return m_tileImage->bitmap().info();
}

void BitmapPattern::drawBitmapToCanvas(SkCanvas& canvas, SkPaint& paint)
{
    canvas.drawBitmap(m_tileImage->bitmap(), SkIntToScalar(0), SkIntToScalar(0), &paint);
}

} // namespace
