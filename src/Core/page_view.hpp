
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2020 Igor Mironchik

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED

// Qt include.
#include <QGraphicsView>
#include <QScopedPointer>


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
class PageView
	:	public QGraphicsView
{
	Q_OBJECT

signals:
	//! Zoom changed.
	void zoomChanged();

public:
	PageView( Cfg::Page & cfg, QWidget * parent = 0 );
	~PageView();

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

	QScopedPointer< PageViewPrivate > d;
}; // class PageView

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED
