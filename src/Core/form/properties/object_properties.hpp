
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_OBJECT_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_OBJECT_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QWidget>

// C++ include.
#include <memory>

// Prototyper include.
#include "project_cfg.hpp"

namespace Ui
{
class ObjectProperties;
}

namespace Prototyper
{

namespace Core
{

class FormObject;

//
// ObjectProperties
//

class ObjectPropertiesPrivate;

//! Properties of the object on the form.
class ObjectProperties final : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectProperties(FormObject *obj,
                              QWidget *parent = nullptr);
    ~ObjectProperties() override;

    Ui::ObjectProperties *ui() const;

    //! Connect properties signals/slots.
    void connectProperties();
    //! Disconnect properties signals/slots.
    void disconnectProperties();

private:
    Q_DISABLE_COPY(ObjectProperties)

    std::unique_ptr<ObjectPropertiesPrivate> d;
}; // class ObjectProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_OBJECT_PROPERTIES_HPP__INCLUDED
