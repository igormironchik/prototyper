
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_TEXT_STYLE_PROPERTIES_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_STYLE_PROPERTIES_HPP__INCLUDED

// Qt include.
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

// Prototyper include.
#include "../undo_commands.hpp"
#include "../utils.hpp"
#include "project_cfg.hpp"
#include "ui_text_style_properties.h"

namespace Prototyper
{

namespace Core
{

//
// FormTextStyleProperties
//

//! Properties of the Text on the form.
class TextStyleProperties final : public QWidget
{
    Q_OBJECT

public:
    TextStyleProperties(QWidget *parent = Q_NULLPTR);
    ~TextStyleProperties() override;

    Ui::TextStyleProperties *ui();

    //! Connect properties signals/slots.
    template<typename T>
    void connectProperties(T *owner)
    {
        connect(m_ui.m_size, QOverload<int>::of(&QSpinBox::valueChanged), owner->q, [this, owner](int v) {
            const auto oldText = owner->q->text();
            owner->m_font.setPixelSize(MmPx::instance().fromPtY(v));
            owner->q->page()->emitChanged();

            owner->q->page()->undoStack()->push(
                new UndoChangeTextWithOpts(owner->q->page(), owner->q->objectId(), oldText, owner->q->text()));

            owner->q->update();
        });

        connect(m_ui.m_bold, &QCheckBox::checkStateChanged, owner->q, [this, owner](Qt::CheckState v) {
            const auto oldText = owner->q->text();
            owner->m_font.setWeight((v == Qt::Checked ? QFont::Bold : QFont::Normal));
            owner->q->page()->emitChanged();

            owner->q->page()->undoStack()->push(
                new UndoChangeTextWithOpts(owner->q->page(), owner->q->objectId(), oldText, owner->q->text()));

            owner->q->update();
        });

        connect(m_ui.m_italic, &QCheckBox::checkStateChanged, owner->q, [this, owner](Qt::CheckState v) {
            const auto oldText = owner->q->text();
            owner->m_font.setItalic((v == Qt::Checked));
            owner->q->page()->emitChanged();

            owner->q->page()->undoStack()->push(
                new UndoChangeTextWithOpts(owner->q->page(), owner->q->objectId(), oldText, owner->q->text()));

            owner->q->update();
        });

        connect(m_ui.m_underline, &QCheckBox::checkStateChanged, owner->q, [this, owner](Qt::CheckState v) {
            const auto oldText = owner->q->text();
            owner->m_font.setUnderline((v == Qt::Checked));
            owner->q->page()->emitChanged();

            owner->q->page()->undoStack()->push(
                new UndoChangeTextWithOpts(owner->q->page(), owner->q->objectId(), oldText, owner->q->text()));

            owner->q->update();
        });
    }

    //! Disconnect properties signals/slots.
    void disconnectProperties();

private:
    Q_DISABLE_COPY(TextStyleProperties)

    //! Ui.
    Ui::TextStyleProperties m_ui;
}; // class TextStyleProperties

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_STYLE_PROPERTIES_HPP__INCLUDED
