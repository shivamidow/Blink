// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/FilterDisplayItem.h"

#include "platform/graphics/GraphicsContext.h"
#include "public/platform/WebDisplayItemList.h"

namespace blink {

BeginFilterDisplayItem::BeginFilterDisplayItem(DisplayItemClient client, PassRefPtr<SkImageFilter> imageFilter, const FloatRect& bounds)
    : PairedBeginDisplayItem(client, BeginFilter)
    , m_imageFilter(imageFilter)
    , m_bounds(bounds)
{
}

BeginFilterDisplayItem::BeginFilterDisplayItem(DisplayItemClient client, PassRefPtr<SkImageFilter> imageFilter, const FloatRect& bounds, PassOwnPtr<WebFilterOperations> webFilterOperations)
    : PairedBeginDisplayItem(client, BeginFilter)
    , m_imageFilter(imageFilter)
    , m_webFilterOperations(webFilterOperations)
    , m_bounds(bounds)
{
}

static FloatRect mapImageFilterRect(SkImageFilter* filter, const FloatRect& bounds)
{
    SkRect filterBounds;
    filter->computeFastBounds(bounds, &filterBounds);
    filterBounds.offset(-bounds.x(), -bounds.y());
    return filterBounds;
}

void BeginFilterDisplayItem::replay(GraphicsContext* context)
{
    context->save();

    FloatRect imageFilterBounds = mapImageFilterRect(m_imageFilter.get(), m_bounds);

    context->translate(m_bounds.x(), m_bounds.y());
    context->beginLayer(1, SkXfermode::kSrcOver_Mode, &imageFilterBounds, ColorFilterNone, m_imageFilter.get());
    context->translate(-m_bounds.x(), -m_bounds.y());
}

void BeginFilterDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    list->appendFilterItem(*m_webFilterOperations, m_bounds);
}

bool BeginFilterDisplayItem::drawsContent() const
{
    // A filter with no inputs must produce its own content.
    return m_imageFilter->countInputs() == 0;
}

#ifndef NDEBUG
void BeginFilterDisplayItem::dumpPropertiesAsDebugString(WTF::StringBuilder& stringBuilder) const
{
    DisplayItem::dumpPropertiesAsDebugString(stringBuilder);
    stringBuilder.append(WTF::String::format(", filter bounds: [%f,%f,%f,%f]",
        m_bounds.x(), m_bounds.y(), m_bounds.width(), m_bounds.height()));
}
#endif

void EndFilterDisplayItem::replay(GraphicsContext* context)
{
    context->endLayer();
    context->restore();
}

void EndFilterDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    list->appendEndFilterItem();
}

} // namespace blink
