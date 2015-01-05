/*
 * Copyright (C) 2015 Collabora Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the contents of this file may be used under the terms
 * of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deletingthe provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#include "config.h"

#if ENABLE(JPEGXR)

#include "platform/image-decoders/jpegxr/JPEGXRImageDecoder.h"

#include "platform/PlatformInstrumentation.h"
#include "wtf/PassOwnPtr.h"

#include <JXRGlue.h>

namespace blink {

static const char* errorToString(const int error) {
    switch(error) {
    case WMP_errNotYetImplemented:
        return "Not yet implemented";
    case WMP_errAbstractMethod :
        return "Abstract method";
    case WMP_errOutOfMemory:
        return "Out of memory";
    case WMP_errFileIO:
        return "File I/O error";
    case WMP_errBufferOverflow:
        return "Buffer overflow";
    case WMP_errInvalidParameter:
        return "Invalid parameter";
    case WMP_errInvalidArgument:
        return "Invalid argument";
    case WMP_errUnsupportedFormat:
        return "Unsupported format";
    case WMP_errIncorrectCodecVersion:
        return "Incorrect codec version";
    case WMP_errIndexNotFound:
        return "Index not found";
    case WMP_errOutOfSequence:
        return "Out of sequence";
    case WMP_errNotInitialized:
        return "Not initialized";
    case WMP_errMustBeMultipleOf16LinesUntilLastCall:
        return "Must be multiple of 16 lines until last call";
    case WMP_errPlanarAlphaBandedEncRequiresTempFile:
        return "Planar alpha banded encoder requires temp files";
    case WMP_errAlphaModeCannotBeTranscoded:
        return "Alpha mode cannot be transcoded";
    case WMP_errIncorrectCodecSubVersion:
        return "Incorrect codec sub version";
    case WMP_errFail:
    default:
        return "Bad instrunction";
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

static ERR writeSource(PKImageEncode* encoder, PKFormatConverter* formatConverter, PKRect* rect)
{
    if (!formatConverter || !formatConverter->pDecoder)
        return WMP_errInvalidParameter;

    // We just needs RGB or RGBA raw data so that we don't need to write a file header.
    encoder->fHeaderDone = true;

    const bool hasAlpha = formatConverter->pDecoder->WMP.bHasAlpha;
    encoder->WMP.bHasAlpha = hasAlpha;
    encoder->cbPixel = hasAlpha ? 4 : 3;

    return PKImageEncode_WriteSource(encoder, formatConverter, rect);
}

static ERR writePixels(PKImageEncode* encoder, U32 line, U8* buffer, U32 stride)
{
    size_t bytesPerLine = encoder->cbPixel * encoder->uWidth;
    if (stride < bytesPerLine)
        return WMP_errInvalidParameter;

    WMPStream* stream = encoder->pStream;
    ERR error = stream->Write(stream, buffer, stream->state.buf.cbBuf);
    if (error < 0)
        return error;

    encoder->idxCurrentLine += line;
    return WMP_errSuccess;
}

class JPEGXRImageReader {
    WTF_MAKE_FAST_ALLOCATED;
public:
    JPEGXRImageReader(JPEGXRImageDecoder* decoder)
        : m_decoder(decoder)
        , m_jxrStreamFactory(createStreamFactory())
        , m_jxrCodecFactory(createCodecFactory())
        , m_jxrDecoder(nullptr)
        , m_jxrEncoder(nullptr)
        , m_jxrFormatConverter(nullptr)
    {
    }

    ~JPEGXRImageReader()
    {
        close();
    }

    PKImageEncode* encoder() { return m_jxrEncoder; }

    void close()
    {
        if (m_jxrDecoder)
            m_jxrDecoder->Release(&m_jxrDecoder);
        if (m_jxrEncoder)
            m_jxrEncoder->Release(&m_jxrEncoder);
        if (m_jxrFormatConverter)
            m_jxrFormatConverter->Release(&m_jxrFormatConverter);

        m_jxrCodecFactory->Release(&m_jxrCodecFactory);
        m_jxrStreamFactory->Release(&m_jxrStreamFactory);
    }

    bool decode(const SharedBuffer& data, bool sizeOnly)
    {
        ASSERT(!m_jxrDecoder);
        m_jxrDecoder = createDecoder(const_cast<char*>(data.data()), data.size());
        if (!m_jxrDecoder)
            return m_decoder->setFailed();

        IntSize size(m_jxrDecoder->uWidth, m_jxrDecoder->uHeight);
        if ((size != m_decoder->size()) && !m_decoder->setSize(size.width(), size.height()))
            return false;

        if (sizeOnly && m_decoder->ImageDecoder::isSizeAvailable())
            return true;

        ASSERT(!m_jxrFormatConverter);
        const bool hasAlpha = m_jxrDecoder->WMP.bHasAlpha;
        PKPixelFormatGUID pixelFormat = hasAlpha ? GUID_PKPixelFormat32bppRGBA : GUID_PKPixelFormat24bppRGB;
        m_jxrFormatConverter = createFormatConverter(m_jxrDecoder, pixelFormat);
        if (!m_jxrFormatConverter)
            return m_decoder->setFailed();

        ASSERT(!m_jxrEncoder);
        size_t bufferSize = size.area() * (hasAlpha ? 4 : 3);
        OwnPtr<char []> buffer = adoptArrayPtr(new char [bufferSize]);
        m_jxrEncoder = createEncoder(buffer.get(), bufferSize);
        if (!m_jxrEncoder)
            return m_decoder->setFailed();

        m_jxrEncoder->SetPixelFormat(m_jxrEncoder, pixelFormat);

        float resolutionX = 0.0, resolutionY = 0.0;
        m_jxrDecoder->GetResolution(m_jxrDecoder, &resolutionX, &resolutionY);
        m_jxrEncoder->SetResolution(m_jxrEncoder, resolutionX, resolutionY);

        PKRect rect = {0, 0, size.width(), size.height()};
        m_jxrEncoder->SetSize(m_jxrEncoder, rect.Width, rect.Height);

        ERR error = m_jxrEncoder->WriteSource(m_jxrEncoder, m_jxrFormatConverter, &rect);
        if (error < 0) {
            WTF_LOG_ERROR("%s", errorToString(error));
            return m_decoder->setFailed();
        }

        m_decoder->outputDecodedData(m_jxrEncoder->pStream->state.buf.pbBuf, size, hasAlpha);

        if (m_jxrEncoder->idxCurrentLine != static_cast<unsigned>(size.height()))
            return m_decoder->setFailed();

        m_decoder->complete();
        return true;
    }

    JPEGXRImageDecoder* decoder() { return m_decoder; }

private:
    PKFactory* createStreamFactory()
    {
        PKFactory* streamFactory = nullptr;
        ERR error = PKCreateFactory(&streamFactory, PK_SDK_VERSION);
        if (error < 0)
            WTF_LOG_ERROR("%s", errorToString(error));

        ASSERT(streamFactory);
        return streamFactory;
    }

    PKCodecFactory* createCodecFactory()
    {
        PKCodecFactory* codecFactory = nullptr;
        ERR error = PKCreateCodecFactory(&codecFactory, WMP_SDK_VERSION);
        if (error < 0)
            WTF_LOG_ERROR("%s", errorToString(error));

        ASSERT(codecFactory);
        return codecFactory;
    }

    WMPStream* createStreamFromMemory(char* data, size_t size)
    {
        WMPStream* stream = nullptr;
        ERR error = m_jxrStreamFactory->CreateStreamFromMemory(&stream, data, size);
        if (error < 0)
            WTF_LOG_ERROR("%s", errorToString(error));

        return stream;
    }

    void* createCodec(const PKIID* id)
    {
        void* codec = nullptr;
        ERR error = m_jxrCodecFactory->CreateCodec(id, &codec);
        if (error < 0) {
            WTF_LOG_ERROR("%s", errorToString(error));
            return nullptr;
        }

        return codec;
    }

    bool initializeDecoder(PKImageDecode* decoder, WMPStream* stream)
    {
        ERR error = decoder->Initialize(decoder, stream);
        if (error < 0) {
            WTF_LOG_ERROR("%s", errorToString(error));
            return false;
        }

        // Free WMPStream where a decoder is destroyed.
        decoder->fStreamOwner = true;

        // 0 : no alpha, 1 : alpha only, else : something + alpha
        decoder->WMP.wmiSCP.uAlphaMode = decoder->WMP.bHasAlpha ? 2 : 0;

        // Set up extra configurations manually or use WMPhotoValidate.
        // WMPhotoValidate(&decoder->WMP.wmiI, &decoder->WMP.wmiSCP);
        CWMImageInfo& imageInfo = decoder->WMP.wmiI;
        imageInfo.cThumbnailWidth = imageInfo.cWidth;
        imageInfo.cThumbnailHeight = imageInfo.cHeight;

        imageInfo.cROILeftX = 0;
        imageInfo.cROITopY = 0;
        imageInfo.cROIWidth = imageInfo.cThumbnailWidth;
        imageInfo.cROIHeight = imageInfo.cThumbnailHeight;

        return true;
    }

    PKImageDecode* createDecoder(char* buffer, size_t bufferSize)
    {
        const PKIID* id = nullptr;
        String filenameExtension = String(".") + m_decoder->filenameExtension();
        ERR error = GetImageDecodeIID(filenameExtension.utf8().data(), &id);
        if (error < 0) {
            WTF_LOG_ERROR("%s", errorToString(error));
            return nullptr;
        }

        PKImageDecode* decoder = static_cast<PKImageDecode*>(createCodec(id));
        if (decoder) {
            WMPStream* stream = createStreamFromMemory(buffer, bufferSize);
            if (!stream) {
                decoder->Release(&decoder);
                return nullptr;
            }
            if (!initializeDecoder(decoder, stream)) {
                stream->Close(&stream);
                decoder->Release(&decoder);
            }
        }
        return decoder;
    }

    bool initializeEncoder(PKImageEncode* encoder, WMPStream* stream)
    {
        ERR error = encoder->Initialize(encoder, stream, nullptr, 0);
        if (error < 0) {
            WTF_LOG_ERROR("%s", errorToString(error));
            return false;
        }

        // Set up extra configurations manually.
        encoder->WriteSource = writeSource;
        encoder->WritePixels = writePixels;

        return true;
    }

    PKImageEncode* createEncoder(char* buffer, size_t bufferSize)
    {
        // We create a custom encoder to ouput RGB or RGBA raw data from decoded stream.
        WMPStream* stream = createStreamFromMemory(buffer, bufferSize);
        if (!stream)
            return nullptr;

        PKImageEncode* encoder = nullptr;
        ERR error = PKImageEncode_Create(&encoder);
        if (error < 0) {
            stream->Close(&stream);
            WTF_LOG_ERROR("%s", errorToString(error));
            return nullptr;
        }

        if (!initializeEncoder(encoder, stream))
            encoder->Release(&encoder);

        return encoder;
    }

    PKFormatConverter* createFormatConverter(PKImageDecode* decoder, PKPixelFormatGUID pixelFormat)
    {
        PKFormatConverter* formatConverter = nullptr;
        ERR error = m_jxrCodecFactory->CreateFormatConverter(&formatConverter);
        if (error < 0) {
            WTF_LOG_ERROR("%s", errorToString(error));
            return nullptr;
        }

        error = formatConverter->Initialize(formatConverter, decoder, nullptr, pixelFormat);
        if (error < 0) {
            formatConverter->Release(&formatConverter);
            WTF_LOG_ERROR("%s", errorToString(error));
        }
        return formatConverter;
    }

    JPEGXRImageDecoder* m_decoder;

    PKFactory* m_jxrStreamFactory;
    PKCodecFactory* m_jxrCodecFactory;
    PKImageDecode* m_jxrDecoder;
    PKImageEncode* m_jxrEncoder;
    PKFormatConverter* m_jxrFormatConverter;
};

JPEGXRImageDecoder::JPEGXRImageDecoder(ImageSource::AlphaOption alphaOption,
    ImageSource::GammaAndColorProfileOption gammaAndColorProfileOption,
    size_t maxDecodedBytes)
    : ImageDecoder(alphaOption, gammaAndColorProfileOption, maxDecodedBytes)
    , m_hasColorProfile(false)
{
}

JPEGXRImageDecoder::~JPEGXRImageDecoder()
{
}

bool JPEGXRImageDecoder::isSizeAvailable()
{
    if (!ImageDecoder::isSizeAvailable())
         decode(true);

    return ImageDecoder::isSizeAvailable();
}

ImageFrame* JPEGXRImageDecoder::frameBufferAtIndex(size_t index)
{
    if (index)
        return 0;

    if (m_frameBufferCache.isEmpty()) {
        m_frameBufferCache.resize(1);
        m_frameBufferCache[0].setPremultiplyAlpha(m_premultiplyAlpha);
    }

    ImageFrame& frame = m_frameBufferCache[0];
    if (frame.status() != ImageFrame::FrameComplete) {
        PlatformInstrumentation::willDecodeImage("JXR");
        decode(false);
        PlatformInstrumentation::didDecodeImage();
    }

    frame.notifyBitmapIfPixelsChanged();
    return &frame;
}

void JPEGXRImageDecoder::complete()
{
    if (m_frameBufferCache.isEmpty())
        return;

    m_frameBufferCache[0].setStatus(ImageFrame::FrameComplete);
}

bool JPEGXRImageDecoder::isComplete() const
{
    if (m_frameBufferCache.isEmpty())
        return false;

    return m_frameBufferCache[0].status() == ImageFrame::FrameComplete;
}

bool JPEGXRImageDecoder::outputDecodedData(const unsigned char* data, const IntSize& decodedSize, const bool hasAlpha)
{
    if (m_frameBufferCache.isEmpty())
        return false;

    // Initialize the framebuffer if needed.
    ImageFrame& buffer = m_frameBufferCache[0];
    if (buffer.status() == ImageFrame::FrameEmpty) {
        if (!buffer.setSize(decodedSize.width(), decodedSize.height()))
            return setFailed();

        buffer.setStatus(ImageFrame::FramePartial);
        buffer.setHasAlpha(hasAlpha);

        // For JPEG XRs, the frame always fills the entire image.
        buffer.setOriginalFrameRect(IntRect(IntPoint(), size()));
    }

    unsigned bytesPerPixel = hasAlpha ? 4 : 3;
    ImageFrame::PixelData* pixel = buffer.getAddr(0, 0);
    for (int y = 0; y < decodedSize.height(); ++y) {
        for (int x = 0; x < decodedSize.width(); ++x, data += bytesPerPixel)
            buffer.setRGBA(pixel++, data[0], data[1], data[2], hasAlpha ? data[3] : 0xff);
    }

    buffer.setPixelsChanged(true);
    return true;
}

void JPEGXRImageDecoder::decode(bool onlySize)
{
    if (failed())
        return;

    if (!m_reader)
        m_reader = adoptPtr(new JPEGXRImageReader(this));

    // If we couldn't decode the image but we've received all the data, decoding
    // has failed.
    if (!m_reader->decode(*m_data, onlySize) && isAllDataReceived())
        setFailed();

    // If decoding is done or failed, we don't need the PNGImageReader anymore.
    //if (isComplete() || failed())
    if (isComplete() || failed())
        m_reader.clear();
}

} //namespace blink

#endif // ENABLE(JPEGXR)
