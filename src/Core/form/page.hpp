
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QTextDocument>

// C++ include.
#include <memory>

// Prototyper include.
#include "button.hpp"
#include "checkbox.hpp"
#include "combobox.hpp"
#include "group.hpp"
#include "hslider.hpp"
#include "image.hpp"
#include "line.hpp"
#include "object.hpp"
#include "page_private.hpp"
#include "polyline.hpp"
#include "radio_button.hpp"
#include "rectangle.hpp"
#include "spinbox.hpp"
#include "text.hpp"
#include "types.hpp"
#include "vslider.hpp"

QT_BEGIN_NAMESPACE
class QUndoStack;
class QAction;
QT_END_NAMESPACE

namespace Prototyper
{

namespace Core
{

namespace Cfg
{

class Form;
class Size;
class Page;

} /* namespace Cfg */

class PagePrivate;
class FormLine;
class FormPolyline;
class FormText;
class FormGroup;
class GridSnap;

//
// Page
//

//! Page.
class Page final : public QGraphicsObject, public FormObject
{
    Q_OBJECT

signals:
    //! Changed.
    void changed();

public:
    explicit Page(Cfg::Page &c,
                  QGraphicsItem *parent = 0);
    ~Page() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return PageType;
    }

    //! \return Undo stack.
    QUndoStack *undoStack() const;

    //! \return Size.
    const Cfg::Size &size() const;
    //! Set size.
    void setSize(const Cfg::Size &s);

    //! \return Grid mode.
    GridMode gridMode() const;
    //! Set grid mode.
    void setGridMode(GridMode m);

    //! \return Grid step.
    int gridStep() const;
    //! Set grid step.
    void setGridStep(int s);

    //! \return Configuration.
    Cfg::Page cfg() const;
    //! Set configuration.
    void setCfg(const Cfg::Page &c);

    //! Switch to select mode.
    void switchToSelectMode();
    //! Switch to line drawing mode.
    void switchToDrawingMode();

    //! Enable/disable snap to grid.
    void enableSnap(bool on = true);

    //! \return Snap item.
    GridSnap *snapItem() const;

    //! \return IDs.
    const QStringList &ids() const;

    //! \return Item with the given id.
    QGraphicsItem *findItem(const QString &id);

    //! Group selection.
    void group();
    //! Ungroup selection.
    void ungroup();

    //! Group items.
    FormGroup *group(const QList<QGraphicsItem *> &items,
                     bool pushUndoCommand = true,
                     const QString &id = QString());
    //! Ungroup group.
    void ungroup(FormGroup *g,
                 bool pushUndoCommand = true);

    //! Align vertical top.
    void alignVerticalTop();
    //! Align vertical center.
    void alignVerticalCenter();
    //! Align vertical bottom.
    void alignVerticalBottom();
    //! Align horizontal left.
    void alignHorizontalLeft();
    //! Align horizontal center.
    void alignHorizontalCenter();
    //! Align horizontal right.
    void alignHorizontalRight();

    //! Delete items.
    void deleteItems(const QList<QGraphicsItem *> &items,
                     bool makeUndoCommand = true);

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    static void draw(QPainter *painter,
                     int width,
                     int height,
                     int gridStep,
                     bool drawGrid = true);

    //! Position elements.
    void setPosition(const QPointF &pos,
                     bool pushUndoCommand = true) override;
    //! \return Position of the element.
    QPointF position() const override;

    //! \return Rectangle of the element.
    QRectF rectangle() const override;
    //! Set rectangle.
    void setRectangle(const QRectF &rect,
                      bool pushUndoCommand = true) override;

    //! Emit changed signal.
    void emitChanged();

    //! Next id.
    QString nextId();

    //! Clone object.
    FormObject *clone() const override;

    //! \return Is comment changed?
    bool isCommentChanged() const;
    //! Clear comment changed flag.
    void clearCommentChanged();

    //! \return Max Z index on the page.
    qreal topZ() const;
    //! \return Min Z index on the page.
    qreal bottomZ() const;

    //! Mouse release handle from PageScene.
    void handleMouseReleaseFromScene();

public slots:
    //! Rename form.
    void renameForm(const QString &name);
    //! Clear edit mode in texts.
    void clearEditModeInTexts();

private slots:
    //! Set grid step.
    void slotSetGridStep();
    //! Undo command in text item was added.
    void undoCommandInTextAdded();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

protected:
    friend class UndoAddLineToPoly;
    template<class Elem, class Config>
    friend class UndoCreate;
    template<class Elem, class Config>
    friend class UndoDelete;
    friend class UndoChangeTextOnForm;
    friend class UndoEditPoly;

    //! Remove document from map.
    void removeDocFromMap(QObject *doc);
    //! Update document's item.
    void updateDocItemInMap(QObject *doc,
                            FormText *text);
    //! Set current line.
    void setCurrentLine(FormLine *line);
    //! Set current polyline.
    void setCurrentPolyLine(FormPolyline *line);
    //! Create element.
    template<class Elem>
    FormObject *createElement(const QString &id)
    {
        FormObject *obj = Q_NULLPTR;

        switch (Elem::staticObjectType()) {
        case FormObject::LineType: {
            obj = new FormLine(this, this);
        } break;

        case FormObject::PolylineType: {
            obj = new FormPolyline(this, this);
        } break;

        case FormObject::TextType: {
            obj = new FormText(QRectF(), this, this);
        } break;

        case FormObject::ImageType: {
            obj = new FormImage(this, this);
        } break;

        case FormObject::RectType: {
            obj = new FormRect(this, this);
        } break;

        case FormObject::GroupType: {
            obj = new FormGroup(this, this);
        } break;

        case FormObject::ButtonType: {
            obj = new FormButton(QRectF(), this, this);
        } break;

        case FormObject::ComboBoxType: {
            obj = new FormComboBox(QRectF(), this, this);
        } break;

        case FormObject::RadioButtonType: {
            obj = new FormRadioButton(QRectF(), this, this);
        } break;

        case FormObject::CheckBoxType: {
            obj = new FormCheckBox(QRectF(), this, this);
        } break;

        case FormObject::HSliderType: {
            obj = new FormHSlider(QRectF(), this, this);
        } break;

        case FormObject::VSliderType: {
            obj = new FormVSlider(QRectF(), this, this);
        } break;

        case FormObject::SpinBoxType: {
            obj = new FormSpinBox(QRectF(), this, this);
        } break;

        default:
            break;
        }

        obj->setObjectId(id);

        dynamic_cast<QGraphicsItem *>(obj)->setZValue(d->currentZValue() + 1.0);

        d->m_ids.append(id);

        return obj;
    }

private:
    friend class PagePrivate;

    Q_DISABLE_COPY(Page)

    std::unique_ptr<PagePrivate> d;
}; // class Page

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HPP__INCLUDED
