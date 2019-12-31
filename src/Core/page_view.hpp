
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016 Igor Mironchik

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

public:
	PageView( Cfg::Page & cfg, QWidget * parent = 0 );
	~PageView();

	//! \return Form scene.
	PageScene * formScene() const;

	//! \return Form.
	Page * page() const;

	//! Enable/disable selection.
	void enableSelection( bool on = true );

private:
	Q_DISABLE_COPY( PageView )

	QScopedPointer< PageViewPrivate > d;
}; // class PageView

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_VIEW_HPP__INCLUDED