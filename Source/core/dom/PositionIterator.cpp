/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/dom/PositionIterator.h"

#include "core/editing/htmlediting.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/layout/LayoutBlock.h"

namespace blink {

using namespace HTMLNames;

PositionIterator::operator Position() const
{
    if (m_nodeAfterPositionInAnchor) {
        ASSERT(m_nodeAfterPositionInAnchor->parentNode() == m_anchorNode);
        // FIXME: This check is inadaquete because any ancestor could be ignored by editing
        if (editingIgnoresContent(m_nodeAfterPositionInAnchor->parentNode()))
            return positionBeforeNode(m_anchorNode);
        return positionInParentBeforeNode(*m_nodeAfterPositionInAnchor);
    }
    if (m_anchorNode->hasChildren())
        return lastPositionInOrAfterNode(m_anchorNode);
    return createLegacyEditingPosition(m_anchorNode, m_offsetInAnchor);
}

void PositionIterator::increment()
{
    if (!m_anchorNode)
        return;

    if (m_nodeAfterPositionInAnchor) {
        m_anchorNode = m_nodeAfterPositionInAnchor;
        m_nodeAfterPositionInAnchor = m_anchorNode->firstChild();
        m_offsetInAnchor = 0;
        return;
    }

    if (!m_anchorNode->hasChildren() && m_offsetInAnchor < lastOffsetForEditing(m_anchorNode))
        m_offsetInAnchor = Position::uncheckedNextOffset(m_anchorNode, m_offsetInAnchor);
    else {
        m_nodeAfterPositionInAnchor = m_anchorNode;
        m_anchorNode = m_nodeAfterPositionInAnchor->parentNode();
        m_nodeAfterPositionInAnchor = m_nodeAfterPositionInAnchor->nextSibling();
        m_offsetInAnchor = 0;
    }
}

void PositionIterator::decrement()
{
    if (!m_anchorNode)
        return;

    if (m_nodeAfterPositionInAnchor) {
        m_anchorNode = m_nodeAfterPositionInAnchor->previousSibling();
        if (m_anchorNode) {
            m_nodeAfterPositionInAnchor = nullptr;
            m_offsetInAnchor = m_anchorNode->hasChildren() ? 0 : lastOffsetForEditing(m_anchorNode);
        } else {
            m_nodeAfterPositionInAnchor = m_nodeAfterPositionInAnchor->parentNode();
            m_anchorNode = m_nodeAfterPositionInAnchor->parentNode();
            m_offsetInAnchor = 0;
        }
        return;
    }

    if (m_anchorNode->hasChildren()) {
        m_anchorNode = m_anchorNode->lastChild();
        m_offsetInAnchor = m_anchorNode->hasChildren()? 0: lastOffsetForEditing(m_anchorNode);
    } else {
        if (m_offsetInAnchor)
            m_offsetInAnchor = Position::uncheckedPreviousOffset(m_anchorNode, m_offsetInAnchor);
        else {
            m_nodeAfterPositionInAnchor = m_anchorNode;
            m_anchorNode = m_anchorNode->parentNode();
        }
    }
}

bool PositionIterator::atStart() const
{
    if (!m_anchorNode)
        return true;
    if (m_anchorNode->parentNode())
        return false;
    return (!m_anchorNode->hasChildren() && !m_offsetInAnchor) || (m_nodeAfterPositionInAnchor && !m_nodeAfterPositionInAnchor->previousSibling());
}

bool PositionIterator::atEnd() const
{
    if (!m_anchorNode)
        return true;
    if (m_nodeAfterPositionInAnchor)
        return false;
    return !m_anchorNode->parentNode() && (m_anchorNode->hasChildren() || m_offsetInAnchor >= lastOffsetForEditing(m_anchorNode));
}

bool PositionIterator::atStartOfNode() const
{
    if (!m_anchorNode)
        return true;
    if (!m_nodeAfterPositionInAnchor)
        return !m_anchorNode->hasChildren() && !m_offsetInAnchor;
    return !m_nodeAfterPositionInAnchor->previousSibling();
}

bool PositionIterator::atEndOfNode() const
{
    if (!m_anchorNode)
        return true;
    if (m_nodeAfterPositionInAnchor)
        return false;
    return m_anchorNode->hasChildren() || m_offsetInAnchor >= lastOffsetForEditing(m_anchorNode);
}

bool PositionIterator::isCandidate() const
{
    if (!m_anchorNode)
        return false;

    LayoutObject* renderer = m_anchorNode->layoutObject();
    if (!renderer)
        return false;

    if (renderer->style()->visibility() != VISIBLE)
        return false;

    if (renderer->isBR()) {
        // For br element, the condition
        // |(!m_anchorNode->hasChildren() || m_nodeAfterPositionInAnchor)|
        // corresponds to the condition
        // |m_anchorType != PositionIsAfterAnchor| in Position.isCandaite.
        // Both conditions say this position is not in tail of the element.
        // If conditions lose consitency, VisiblePosition::canonicalPosition
        // will fail on |canonicalizeCandidate(previousCandidate(position))|,
        // because previousCandidate returns a Position converted from
        // a "Candidate" PositionIterator and cannonicalizeCandidate(Position)
        // assumes the Position is "Candidate".
        return !m_offsetInAnchor && (!m_anchorNode->hasChildren() || m_nodeAfterPositionInAnchor) && !Position::nodeIsUserSelectNone(m_anchorNode->parentNode());
    }
    if (renderer->isText())
        return !Position::nodeIsUserSelectNone(m_anchorNode) && Position(*this).inRenderedText();

    if (renderer->isSVG()) {
        // We don't consider SVG elements are contenteditable except for
        // associated renderer returns isText() true, e.g. LayoutSVGInlineText.
        return false;
    }

    if (isRenderedHTMLTableElement(m_anchorNode) || editingIgnoresContent(m_anchorNode))
        return (atStartOfNode() || atEndOfNode()) && !Position::nodeIsUserSelectNone(m_anchorNode->parentNode());

    if (!isHTMLHtmlElement(*m_anchorNode) && renderer->isLayoutBlockFlow()) {
        if (toLayoutBlock(renderer)->logicalHeight() || isHTMLBodyElement(*m_anchorNode)) {
            if (!Position::hasRenderedNonAnonymousDescendantsWithHeight(renderer))
                return atStartOfNode() && !Position::nodeIsUserSelectNone(m_anchorNode);
            return m_anchorNode->hasEditableStyle() && !Position::nodeIsUserSelectNone(m_anchorNode) && Position(*this).atEditingBoundary();
        }
    }

    return false;
}

} // namespace blink
