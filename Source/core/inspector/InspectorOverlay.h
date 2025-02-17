/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorOverlay_h
#define InspectorOverlay_h

#include "core/InspectorTypeBuilder.h"
#include "platform/Timer.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/Color.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Color;
class EmptyChromeClient;
class GraphicsContext;
class InspectorOverlayHost;
class JSONValue;
class Node;
class Page;
class PlatformGestureEvent;
class PlatformKeyboardEvent;
class PlatformMouseEvent;
class PlatformTouchEvent;

struct HighlightConfig {
    WTF_MAKE_FAST_ALLOCATED;
public:
    Color content;
    Color contentOutline;
    Color padding;
    Color border;
    Color margin;
    Color eventTarget;
    Color shape;
    Color shapeMargin;

    bool showInfo;
    bool showRulers;
    bool showExtensionLines;
};

class InspectorOverlay final : public NoBaseWillBeGarbageCollectedFinalized<InspectorOverlay> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED;
public:
    class Client {
    public:
        virtual ~Client() { }

        virtual void highlight() = 0;
        virtual void hideHighlight() = 0;
    };

    static PassOwnPtrWillBeRawPtr<InspectorOverlay> create(Page* page, Client* client)
    {
        return adoptPtrWillBeNoop(new InspectorOverlay(page, client));
    }

    ~InspectorOverlay();
    DECLARE_TRACE();

    void update();
    void hide();
    void paint(GraphicsContext&);
    void drawOutline(GraphicsContext*, const LayoutRect&, const Color&);
    bool handleGestureEvent(const PlatformGestureEvent&);
    bool handleMouseEvent(const PlatformMouseEvent&);
    bool handleTouchEvent(const PlatformTouchEvent&);
    bool handleKeyboardEvent(const PlatformKeyboardEvent&);

    void setPausedInDebuggerMessage(const String*);
    void setInspectModeEnabled(bool);

    void hideHighlight();
    void highlightNode(Node*, Node* eventTarget, const HighlightConfig&, bool omitTooltip);
    void highlightQuad(PassOwnPtr<FloatQuad>, const HighlightConfig&);
    void showAndHideViewSize(bool showGrid);

    Node* highlightedNode() const;
    bool getBoxModel(Node*, RefPtr<TypeBuilder::DOM::BoxModel>&);
    PassRefPtr<JSONObject> highlightJSONForNode(Node*);

    void freePage();

    InspectorOverlayHost* overlayHost() const { return m_overlayHost.get(); }

    void startedRecordingProfile();
    void finishedRecordingProfile() { m_activeProfilerCount--; }

    // Methods supporting underlying overlay page.
    void invalidate();
private:
    InspectorOverlay(Page*, Client*);

    bool isEmpty();

    void drawNodeHighlight();
    void drawQuadHighlight();
    void drawPausedInDebuggerMessage();
    void drawViewSize();

    Page* overlayPage();
    void reset(const IntSize& viewportSize, int scrollX, int scrollY);
    void evaluateInOverlay(const String& method, const String& argument);
    void evaluateInOverlay(const String& method, PassRefPtr<JSONValue> argument);
    void onTimer(Timer<InspectorOverlay>*);

    RawPtrWillBeMember<Page> m_page;
    Client* m_client;
    String m_pausedInDebuggerMessage;
    bool m_inspectModeEnabled;
    RefPtrWillBeMember<Node> m_highlightNode;
    RefPtrWillBeMember<Node> m_eventTargetNode;
    HighlightConfig m_nodeHighlightConfig;
    OwnPtr<FloatQuad> m_highlightQuad;
    OwnPtrWillBeMember<Page> m_overlayPage;
    OwnPtr<EmptyChromeClient> m_overlayChromeClient;
    RefPtrWillBeMember<InspectorOverlayHost> m_overlayHost;
    HighlightConfig m_quadHighlightConfig;
    bool m_drawViewSize;
    bool m_drawViewSizeWithGrid;
    bool m_omitTooltip;
    Timer<InspectorOverlay> m_timer;
    int m_activeProfilerCount;
    bool m_updating;
};

} // namespace blink


#endif // InspectorOverlay_h
