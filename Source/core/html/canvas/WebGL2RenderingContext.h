// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGL2RenderingContext_h
#define WebGL2RenderingContext_h

#include "core/html/canvas/WebGL2RenderingContextBase.h"

namespace blink {

class WebGL2RenderingContext : public WebGL2RenderingContextBase {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassOwnPtrWillBeRawPtr<WebGL2RenderingContext> create(HTMLCanvasElement*, const CanvasContextCreationAttributes&);
    ~WebGL2RenderingContext() override;

    unsigned version() const override { return 2; }
    String contextName() const override { return "WebGL2RenderingContext"; }
    void registerContextExtensions() override;

    DECLARE_VIRTUAL_TRACE();

protected:
    WebGL2RenderingContext(HTMLCanvasElement* passedCanvas, PassOwnPtr<blink::WebGraphicsContext3D>, const WebGLContextAttributes& requestedAttributes);

    RefPtrWillBeMember<CHROMIUMSubscribeUniform> m_chromiumSubscribeUniform;
    RefPtrWillBeMember<EXTTextureFilterAnisotropic> m_extTextureFilterAnisotropic;
    RefPtrWillBeMember<OESTextureFloatLinear> m_oesTextureFloatLinear;
    RefPtrWillBeMember<WebGLCompressedTextureATC> m_webglCompressedTextureATC;
    RefPtrWillBeMember<WebGLCompressedTextureETC1> m_webglCompressedTextureETC1;
    RefPtrWillBeMember<WebGLCompressedTexturePVRTC> m_webglCompressedTexturePVRTC;
    RefPtrWillBeMember<WebGLCompressedTextureS3TC> m_webglCompressedTextureS3TC;
    RefPtrWillBeMember<WebGLDebugRendererInfo> m_webglDebugRendererInfo;
    RefPtrWillBeMember<WebGLDebugShaders> m_webglDebugShaders;
    RefPtrWillBeMember<WebGLLoseContext> m_webglLoseContext;
};

DEFINE_TYPE_CASTS(WebGL2RenderingContext, CanvasRenderingContext, context,
    context->is3d() && WebGLRenderingContextBase::getWebGLVersion(context) == 2,
    context.is3d() && WebGLRenderingContextBase::getWebGLVersion(&context) == 2);

} // namespace blink

#endif
