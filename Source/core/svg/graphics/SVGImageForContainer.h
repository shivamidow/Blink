/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SVGImageForContainer_h
#define SVGImageForContainer_h

#include "core/svg/graphics/SVGImage.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/FloatSize.h"
#include "platform/graphics/Image.h"
#include "platform/weborigin/KURL.h"

namespace blink {

class SVGImageForContainer final : public Image {
public:
    static PassRefPtr<SVGImageForContainer> create(SVGImage* image, const FloatSize& containerSize, float zoom)
    {
        return adoptRef(new SVGImageForContainer(image, containerSize, zoom));
    }

    virtual bool isSVGImage() const override { return true; }

    virtual IntSize size() const override;
    void setURL(const KURL& url) { m_image->setURL(url); }

    virtual bool usesContainerSize() const override { return m_image->usesContainerSize(); }
    virtual bool hasRelativeWidth() const override { return m_image->hasRelativeWidth(); }
    virtual bool hasRelativeHeight() const override { return m_image->hasRelativeHeight(); }
    virtual void computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) override
    {
        m_image->computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
    }

    void draw(GraphicsContext*, const FloatRect&, const FloatRect&, SkXfermode::Mode, RespectImageOrientationEnum) override;

    virtual void drawPattern(GraphicsContext*, const FloatRect&, const FloatSize&, const FloatPoint&, SkXfermode::Mode, const FloatRect&, const IntSize& repeatSpacing) override;

    // FIXME: Implement this to be less conservative.
    virtual bool currentFrameKnownToBeOpaque() override { return false; }

    virtual PassRefPtr<NativeImageSkia> nativeImageForCurrentFrame() override;

private:
    SVGImageForContainer(SVGImage* image, const FloatSize& containerSize, float zoom)
        : m_image(image)
        , m_containerSize(containerSize)
        , m_zoom(zoom)
    {
    }

    virtual void destroyDecodedData(bool) override { }

    SVGImage* m_image;
    const FloatSize m_containerSize;
    const float m_zoom;
};
}

#endif // SVGImageForContainer_h
