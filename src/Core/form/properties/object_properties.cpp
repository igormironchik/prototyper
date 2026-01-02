
/*
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// Prototyper include.
#include "object_properties.hpp"
#include "../object.hpp"
#include "../page.hpp"
#include "../utils.hpp"
#include "ui_object_properties.h"

// Qt include.
#include <QCheckBox>
#include <QSpacerItem>
#include <QSpinBox>
#include <QVBoxLayout>

namespace Prototyper
{

namespace Core
{

//
// ObjectPropertiesPrivate
//

class ObjectPropertiesPrivate
{
public:
    explicit ObjectPropertiesPrivate(FormObject *obj,
                                     ObjectProperties *parent)
        : q(parent)
        , m_obj(obj)
    {
    }

    //! Init.
    void init();

    //! Parent.
    ObjectProperties *q;
    //! Form's object.
    FormObject *m_obj;
    //! Ui.
    Ui::ObjectProperties m_ui;
}; // class ObjectPropertiesPrivate

void ObjectPropertiesPrivate::init()
{
    m_ui.setupUi(q);
}

//
// ObjectProperties
//

ObjectProperties::ObjectProperties(FormObject *obj,
                                   QWidget *parent)
    : QWidget(parent)
    , d(new ObjectPropertiesPrivate(obj,
                                    this))
{
    d->init();
}

ObjectProperties::~ObjectProperties() = default;

Ui::ObjectProperties *ObjectProperties::ui() const
{
    return &d->m_ui;
}

void ObjectProperties::connectProperties()
{
    connect(d->m_ui.m_x, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        d->m_obj->setPosition(QPointF(v, d->m_obj->position().y()));
        d->m_obj->page()->emitChanged();
    });

    connect(d->m_ui.m_y, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        d->m_obj->setPosition(QPointF(d->m_obj->position().x(), v));
        d->m_obj->page()->emitChanged();
    });

    connect(d->m_ui.m_width, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        QRectF r = d->m_obj->rectangle();
        r.setWidth(v);
        d->m_obj->setRectangle(r, true);
        d->m_obj->page()->emitChanged();
    });

    connect(d->m_ui.m_height, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        QRectF r = d->m_obj->rectangle();
        r.setHeight(v);
        d->m_obj->setRectangle(r, true);
        d->m_obj->page()->emitChanged();
    });
}

void ObjectProperties::disconnectProperties()
{
    disconnect(d->m_ui.m_x, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    disconnect(d->m_ui.m_y, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    disconnect(d->m_ui.m_width, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);

    disconnect(d->m_ui.m_height, QOverload<int>::of(&QSpinBox::valueChanged), nullptr, nullptr);
}

} /* namespace Core */

} /* namespace Prototyper */
