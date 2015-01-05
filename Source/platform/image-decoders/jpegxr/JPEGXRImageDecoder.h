/*
 * Copyright (C) 2015 Collabora Ltd.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JPEGXRImageDecoder_h
#define JPEGXRImageDecoder_h

#if ENABLE(JPEGXR)

#include "platform/image-decoders/ImageDecoder.h"

#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"

namespace blink {

class JPEGXRImageReader;

// This class decodes the JPEG image format.
class PLATFORM_EXPORT JPEGXRImageDecoder : public ImageDecoder {
    WTF_MAKE_NONCOPYABLE(JPEGXRImageDecoder);
public:
    JPEGXRImageDecoder(ImageSource::AlphaOption, ImageSource::GammaAndColorProfileOption, size_t maxDecodedBytes);
    virtual ~JPEGXRImageDecoder();

    // ImageDecoder
    virtual String filenameExtension() const override { return "jxr"; }
    virtual bool isSizeAvailable() override;
    virtual bool hasColorProfile() const override { return m_hasColorProfile; }
    virtual ImageFrame* frameBufferAtIndex(size_t) override;

    void complete();
    bool isComplete() const;
    bool outputDecodedData(const unsigned char* data, const IntSize&, const bool hasAlpha);

private:
    void decode(bool onlySize);

    OwnPtr<JPEGXRImageReader> m_reader;
    bool m_hasColorProfile;
};

} // namespace blink

#endif // ENABLE(JPEGXR)

#endif // JPEGXRImageDecoder_h
