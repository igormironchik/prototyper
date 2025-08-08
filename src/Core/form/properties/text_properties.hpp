
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_TEXT_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

// Prototyper include.
#include "../undo_commands.hpp"
#include "../utils.hpp"
#include "project_cfg.hpp"
#include "ui_text_properties.h"

namespace Prototyper
{

namespace Core
{

//
// FormTextProperties
//

//! Properties of the Text on the form.
class TextProperties final : public QWidget
{
    Q_OBJECT

public:
    TextProperties(QWidget *parent = Q_NULLPTR);
    ~TextProperties() override;

    Ui::TextProperties *ui();

    //! Connect properties signals/slots.
    template<typename T>
    void connectProperties(T *owner)
    {
        connect(m_ui.m_text, &QLineEdit::textChanged, owner->q, [this, owner](const QString &t) {
            const auto oldText = owner->q->text();
            owner->m_text = t;
            owner->q->page()->emitChanged();

            owner->q->page()->undoStack()->push(
                new UndoChangeTextWithOpts(owner->q->page(), owner->q->objectId(), oldText, owner->q->text()));

            owner->q->update();
        });
    }

    //! Disconnect properties signals/slots.
    void disconnectProperties();

private:
    Q_DISABLE_COPY(TextProperties)

    //! Ui.
    Ui::TextProperties m_ui;
}; // class TextProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_PROPERTIES_HPP__INCLUDED
