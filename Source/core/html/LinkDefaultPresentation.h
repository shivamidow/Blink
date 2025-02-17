// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LinkDefaultPresentation_h
#define LinkDefaultPresentation_h

#include "core/html/LinkResource.h"
#include "wtf/FastAllocBase.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class HTMLLinkElement;

// Represents the default presentation URL resource associated with the <link> tag.
// Notifies the WebFrameClient about the change of its value.
class LinkDefaultPresentation final : public LinkResource {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED;
public:

    static PassOwnPtrWillBeRawPtr<LinkDefaultPresentation> create(HTMLLinkElement* owner);

    virtual ~LinkDefaultPresentation();

    // LinkResource
    virtual void process() override;
    virtual Type type() const override { return Manifest; }
    virtual bool hasLoaded() const override;
    virtual void ownerRemoved() override;

private:
    explicit LinkDefaultPresentation(HTMLLinkElement* owner);
};

}

#endif // LinkDefaultPresentation_h
