// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorPlugins_h
#define NavigatorPlugins_h

#include "core/frame/DOMWindowProperty.h"
#include "platform/Supplementable.h"

namespace blink {

class DOMMimeTypeArray;
class DOMPluginArray;
class LocalFrame;
class Navigator;

class NavigatorPlugins final
    : public NoBaseWillBeGarbageCollected<NavigatorPlugins>
    , public WillBeHeapSupplement<Navigator>
    , DOMWindowProperty {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(NavigatorPlugins);
    DECLARE_EMPTY_VIRTUAL_DESTRUCTOR_WILL_BE_REMOVED(NavigatorPlugins);
public:
    static NavigatorPlugins& from(Navigator&);
    static NavigatorPlugins* toNavigatorPlugins(Navigator&);
    static const char* supplementName();

    static DOMPluginArray* plugins(Navigator&);
    static DOMMimeTypeArray* mimeTypes(Navigator&);
    static bool javaEnabled(Navigator&);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit NavigatorPlugins(Navigator&);

    DOMPluginArray* plugins(LocalFrame*) const;
    DOMMimeTypeArray* mimeTypes(LocalFrame*) const;
    bool javaEnabled(LocalFrame*) const;

    mutable RefPtrWillBeMember<DOMPluginArray> m_plugins;
    mutable RefPtrWillBeMember<DOMMimeTypeArray> m_mimeTypes;
};

} // namespace blink

#endif // NavigatorPlugins_h
