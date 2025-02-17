// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TableCellPainter_h
#define TableCellPainter_h

#include "core/layout/style/CollapsedBorderValue.h"

namespace blink {

struct PaintInfo;
class LayoutPoint;
class LayoutRect;
class LayoutTableCell;
class LayoutObject;
class LayoutStyle;

class TableCellPainter {
public:
    TableCellPainter(LayoutTableCell& layoutTableCell) : m_layoutTableCell(layoutTableCell) { }

    void paint(const PaintInfo&, const LayoutPoint&);

    void paintCollapsedBorders(const PaintInfo&, const LayoutPoint&);
    void paintBackgroundsBehindCell(const PaintInfo&, const LayoutPoint&, LayoutObject* backgroundObject);
    void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint& paintOffset);
    void paintMask(const PaintInfo&, const LayoutPoint& paintOffset);

    enum PaintBoundOffsetBehavior { AddOffsetFromParent, DoNotAddOffsetFromParent };
    // Returns the bonds of the table cell for painting, offset by paintOffset, and if desired, the offset from the cell
    // to its parent.
    LayoutRect paintBounds(const LayoutPoint& paintOffset, PaintBoundOffsetBehavior);

private:
    CollapsedBorderValue cachedCollapsedLeftBorder(const LayoutStyle&) const;
    CollapsedBorderValue cachedCollapsedRightBorder(const LayoutStyle&) const;
    CollapsedBorderValue cachedCollapsedTopBorder(const LayoutStyle&) const;
    CollapsedBorderValue cachedCollapsedBottomBorder(const LayoutStyle&) const;

    LayoutTableCell& m_layoutTableCell;
};

} // namespace blink

#endif // TableCellPainter_h
