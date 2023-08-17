
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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

#ifndef PROTOTYPER__CORE__FORM_SCENE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_SCENE_HPP__INCLUDED

// Qt include.
#include <QGraphicsScene>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

namespace Cfg {

class Page;

} /* namespace Cfg */

class Page;


//
// PageScene
//

class PageScenePrivate;

//! Page scene.
class PageScene final
	:	public QGraphicsScene
{
	Q_OBJECT

signals:
	//! Scene changed.
	void changed();

public:
	PageScene( const Cfg::Page & c, QObject * parent = 0 );
	~PageScene() override;

	//! \return Page.
	Page * page() const;
	//! Set page.
	void setPage( Page * f );

public slots:
	//! Delete selected items.
	void deleteSelected();
	//! Enable selection.
	void enableSelection( bool on = true );

protected:
	void keyPressEvent( QKeyEvent * event ) override;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * event ) override;
	void mousePressEvent( QGraphicsSceneMouseEvent * event ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) override;
	void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event ) override;

private:
	Q_DISABLE_COPY( PageScene )

	std::unique_ptr< PageScenePrivate > d;
}; // class PageScene

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_SCENE_HPP__INCLUDED
