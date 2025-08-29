
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_Pixmap_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_Pixmap_HPP__INCLUDED

// Qt include.
#include <QGraphicsPixmapItem>

// C++ include.
#include <memory>

// Prototyper include.
#include "object.hpp"
#include "project_cfg.hpp"
#include "resizable.hpp"

namespace Prototyper
{

namespace Core
{

class Page;

//
// FormImage
//

class FormImagePrivate;

//! Pixmap on the form.
class FormImage final : public QGraphicsPixmapItem, public FormObject, public FormResizable
{
public:
    explicit FormImage(Page *page,
                       QGraphicsItem *parent = 0);
    ~FormImage() override;

    //! \return Type.
    static ObjectType staticObjectType()
    {
        return ImageType;
    }

    //! \return Cfg.
    Cfg::Image cfg() const;
    //! Set cfg.
    void setCfg(const Cfg::Image &c);

    //! \return Image.
    const QImage &image() const;
    //! Set image.
    void setImage(const QImage &img);

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

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

    //! Clone object.
    FormObject *clone() const override;

    //! \return Widget with properties of object.
    QWidget *properties(QWidget *parent) override;

    //! Update values of properties.
    void updatePropertiesValues() override;

protected:
    //! Resize.
    void resize(const QRectF &rect) override;
    //! Move resizable.
    void moveResizable(const QPointF &delta) override;

private:
    Q_DISABLE_COPY(FormImage)

    std::unique_ptr<FormImagePrivate> d;
}; // class FormImage

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_Pixmap_HPP__INCLUDED
