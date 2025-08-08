
/*
    SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_SCENE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_SCENE_HPP__INCLUDED

// Qt include.
#include <QGraphicsScene>

// C++ include.
#include <memory>

namespace Prototyper
{

namespace Core
{

namespace Cfg
{

class Page;

} /* namespace Cfg */

class Page;

//
// PageScene
//

class PageScenePrivate;

//! Page scene.
class PageScene final : public QGraphicsScene
{
    Q_OBJECT

signals:
    //! Scene changed.
    void changed();

public:
    PageScene(const Cfg::Page &c,
              QObject *parent = 0);
    ~PageScene() override;

    //! \return Page.
    Page *page() const;
    //! Set page.
    void setPage(Page *f);

public slots:
    //! Delete selected items.
    void deleteSelected();
    //! Enable selection.
    void enableSelection(bool on = true);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    Q_DISABLE_COPY(PageScene)

    std::unique_ptr<PageScenePrivate> d;
}; // class PageScene

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_SCENE_HPP__INCLUDED
