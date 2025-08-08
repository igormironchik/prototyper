
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__PAGE_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__PAGE_PRIVATE_HPP__INCLUDED

// Qt include.
#include <QList>
#include <QMap>
#include <QPointF>

// C++ include.
#include <memory>
#include <vector>

// Prototyper include.
#include "types.hpp"

QT_BEGIN_NAMESPACE
class QUndoStack;
class QAction;
class QGraphicsItem;

template<typename>
class QSharedPointer;

class QTextDocument;
class QRectF;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

namespace Cfg
{

class Page;
class Size;
class Text;
class Group;
class TextStyle;

} /* namespace Cfg */

class Page;
class FormLine;
class FormText;
class FormGroup;
class GridSnap;
class FormPolyline;
class PageComment;

//
// PagePrivate
//

class PagePrivate final
{
public:
    PagePrivate(Cfg::Page &cfg,
                Page *parent)
        : q(parent)
        , m_gridMode(ShowGrid)
        , m_gridStepAction(0)
        , m_cfg(cfg)
        , m_pressed(false)
        , m_current(0)
        , m_id(0)
        , m_currentLine(nullptr)
        , m_snap(0)
        , m_polyline(false)
        , m_isCommentChanged(false)
        , m_currentPoly(0)
        , m_undoStack(0)
    {
    }

    //! Init.
    void init();
    //! \return Current Z-value.
    qreal currentZValue() const;
    //! \return Current Z-value.
    void currentZValue(const QList<QGraphicsItem *> &items,
                       qreal &z,
                       bool initZ) const;
    //! \return Start point for line.
    QPointF lineStartPoint(const QPointF &point,
                           bool &intersected,
                           bool &intersectedEnds) const;
    //! Handle mouse move in current lines.
    void handleMouseMoveInCurrentLine(const QPointF &point);
    //! Handle mouse move in current polyline.
    void handleMouseMoveInCurrentPolyLine(const QPointF &point);
    //! Ungroup.
    void ungroup(QGraphicsItem *group,
                 bool pushUndoCommand = true);
    //! \return Next ID.
    QString id();
    //! Update form from the configuration.
    void updateFromCfg();
    //! Clear form.
    void clear();
    //! Create text.
    FormText *createText(const Cfg::Text &cfg);
    //! Create group.
    FormGroup *createGroup(const Cfg::Group &cfg);
    //! Create element.
    template<class Elem,
             class Config>
    Elem *createElem(const Config &cfg);
    //! Create element with rect.
    template<class Elem,
             class Config>
    Elem *createElemWithRect(const Config &cfg,
                             const QRectF &rect);
    //! Clear IDs.
    void clearIds(FormGroup *group);
    //! Add IDs.
    void addIds(FormGroup *group);
    //! Set text.
    void setText(const QSharedPointer<QTextDocument> &doc,
                 const std::vector<Cfg::TextStyle> &text);
    //! Hide handles of current item.
    void hideHandlesOfCurrent();
    //! Selection.
    QList<QGraphicsItem *> selection();
    //! Is comment under mosue?
    bool isCommentUnderMouse() const;

    //! AlignPoint.
    enum AlignPoint {
        //! Align Horizontal Left.
        AlignHorLeftPoint,
        //! Align Horizontal Center.
        AlignHorCenterPoint,
        //! Align Horizontal Right.
        AlignHorRightPoint,
        //! Align Vertical Top.
        AlignVertTopPoint,
        //! Align Vertical Center.
        AlignVertCenterPoint,
        //! Align Vertical Bottom.
        AlignVertBottomPoint
    }; // enum AlignPoint

    //! Search align point.
    qreal searchAlignPoint(const QList<QGraphicsItem *> &items,
                           AlignPoint point);

    //! Parent.
    Page *q;
    //! Grid mode.
    GridMode m_gridMode;
    //! Grid step action.
    QAction *m_gridStepAction;
    //! Cfg.
    Cfg::Page &m_cfg;
    //! Pressed.
    bool m_pressed;
    //! Current item.
    QGraphicsItem *m_current;
    //! Mouse pos.
    QPointF m_pos;
    //! ID.
    quint64 m_id;
    //! Current lines.
    FormLine *m_currentLine;
    //! Grid snap.
    GridSnap *m_snap;
    //! Make polyline.
    bool m_polyline;
    //! Comment was added/deleted;
    bool m_isCommentChanged;
    //! Current polyline.
    FormPolyline *m_currentPoly;
    //! IDs
    QStringList m_ids;
    //! Undo stack.
    QUndoStack *m_undoStack;
    //! Map of text documents.
    QMap<QObject *, FormText *> m_docs;
    //! Comments.
    QList<PageComment *> m_comments;
}; // class PagePrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PAGE_PRIVATE_HPP__INCLUDED
