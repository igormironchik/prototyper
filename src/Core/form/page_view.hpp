
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED

// Qt include.
#include <QGraphicsView>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

namespace Cfg {

class Page;

} /* namespace Cfg */

class PageScene;
class Page;

//
// PageView
//

class PageViewPrivate;

//! Page view.
class PageView final
	:	public QGraphicsView
{
	Q_OBJECT

signals:
	//! Zoom changed.
	void zoomChanged();

public:
	PageView( const Cfg::Page & cfg, QWidget * parent = 0 );
	~PageView() override;

	//! \return Page scene.
	PageScene * pageScene() const;

	//! \return Form.
	Page * page() const;

	//! Enable/disable selection.
	void enableSelection( bool on = true );

	//! \return Scale.
	qreal scaleValue() const;
	//! Scale.
	void setScaleValue( qreal s );

	static qreal zoomFactor();

protected:
	void wheelEvent( QWheelEvent * event ) override;

private:
	friend class PageViewPrivate;

	Q_DISABLE_COPY( PageView )

	std::unique_ptr< PageViewPrivate > d;
}; // class PageView

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED
