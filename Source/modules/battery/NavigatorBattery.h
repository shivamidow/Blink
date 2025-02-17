// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorBattery_h
#define NavigatorBattery_h

#include "bindings/core/v8/ScriptPromise.h"
#include "core/frame/Navigator.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class BatteryManager;
class Navigator;

class NavigatorBattery final : public NoBaseWillBeGarbageCollectedFinalized<NavigatorBattery>, public WillBeHeapSupplement<Navigator> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(NavigatorBattery);
public:
    virtual ~NavigatorBattery();

    static NavigatorBattery& from(Navigator&);

    static ScriptPromise getBattery(ScriptState*, Navigator&);
    ScriptPromise getBattery(ScriptState*);

    DECLARE_TRACE();

private:
    NavigatorBattery();
    static const char* supplementName();

    PersistentWillBeMember<BatteryManager> m_batteryManager;
};

} // namespace blink

#endif // NavigatorBattery_h
