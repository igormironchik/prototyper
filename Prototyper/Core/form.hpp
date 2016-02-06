
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

#ifndef PROTOTYPER__CORE__FORM_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

namespace Cfg {

class Form;
class Size;

} /* namespace Cfg */

//
// Form
//

class FormPrivate;

//! Form.
class Form
	:	public QGraphicsObject
{
	Q_OBJECT

signals:
	//! Changed.
	void changed();

public:
	explicit Form( Cfg::Form & c, QGraphicsItem * parent = 0 );
	~Form();

	//! Grid mode.
	enum GridMode {
		//! Show grid.
		ShowGrid,
		//! No grid.
		NoGrid
	}; // enum GridMode

	//! \return Size.
	const Cfg::Size & size() const;
	//! Set size.
	void setSize( const Cfg::Size & s );

	//! \return Grid mode.
	GridMode gridMode() const;
	//! Set grid mode.
	void setGridMode( GridMode m );

	//! \return Grid step.
	int gridStep() const;
	//! Set grid step.
	void setGridStep( int s );

	//! \return Configuration.
	const Cfg::Form & cfg() const;

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget ) Q_DECL_OVERRIDE;

private slots:
	//! Set grid step.
	void slotSetGridStep();

protected:
	void contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
		Q_DECL_OVERRIDE;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
		Q_DECL_OVERRIDE;
	void mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent )
		Q_DECL_OVERRIDE;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
		Q_DECL_OVERRIDE;

private:
	friend class FormPrivate;

	Q_DISABLE_COPY( Form )

	QScopedPointer< FormPrivate > d;
}; // class Form

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HPP__INCLUDED
