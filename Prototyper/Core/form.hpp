
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

// Prototyper include.
#include "form_object.hpp"
#include "form_properties.hpp"

QT_BEGIN_NAMESPACE
class QUndoStack;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

namespace Cfg {

class Form;
class Size;

} /* namespace Cfg */

class GridSnap;
class FormGroup;
class FormLine;
class FormPolyline;


//
// Form
//

class FormPrivate;

//! Form.
class Form
	:	public QGraphicsObject
	,	public FormObject
{
	Q_OBJECT

signals:
	//! Changed.
	void changed();

public:
	explicit Form( Cfg::Form & c, QGraphicsItem * parent = 0 );
	~Form();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return FormType; }

	//! Grid mode.
	enum GridMode {
		//! Show grid.
		ShowGrid,
		//! No grid.
		NoGrid
	}; // enum GridMode

	//! \return Undo stack.
	QUndoStack * undoStack() const;

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
	Cfg::Form cfg() const;
	//! Set configuration.
	void setCfg( const Cfg::Form & c );

	//! Switch to select mode.
	void switchToSelectMode();
	//! Switch to line drawing mode.
	void switchToLineDrawingMode();

	//! Enable/disable snap to grid.
	void enableSnap( bool on = true );

	//! \return Snap item.
	GridSnap * snapItem() const;

	//! \return IDs.
	const QStringList & ids() const;

	//! \return Item with the given id.
	QGraphicsItem * findItem( const QString & id );

	//! Group selection.
	void group();
	//! Ungroup selection.
	void ungroup();

	//! Group items.
	FormGroup * group( const QList< QGraphicsItem* > & items );
	//! Ungroup group.
	void ungroup( FormGroup * g );

	//! Align vertical top.
	void alignVerticalTop();
	//! Align vertical center.
	void alignVerticalCenter();
	//! Align vertical bottom.
	void alignVerticalBottom();
	//! Align horizontal left.
	void alignHorizontalLeft();
	//! Align horizontal center.
	void alignHorizontalCenter();
	//! Align horizontal right.
	void alignHorizontalRight();

	//! Delete items.
	void deleteItems( const QList< QGraphicsItem* > & items );

	//! Update renamed link.
	void updateLink( const QString & oldName, const QString & name );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget ) Q_DECL_OVERRIDE;

	static void draw( QPainter * painter, int width, int height,
		FormProperties::Buttons btns, int gridStep, bool drawGrid = true );

	//! Position elements.
	void setPosition( const QPointF & pos ) Q_DECL_OVERRIDE;

	//! \return Position of the element.
	QPointF position() const Q_DECL_OVERRIDE;

	//! \return Rectangle of the element.
	QRectF rectangle() const Q_DECL_OVERRIDE;

	//! Set rectangle.
	void setRectangle( const QRectF & rect ) Q_DECL_OVERRIDE;

public slots:
	//! Rename object.
	void renameObject( FormObject * obj );
	//! Rename object.
	void renameObject( FormObject * obj, const QString & newId );
	//! Rename form.
	void renameForm( const QString & name );
	//! Edit description.
	void editDescription( const QString & id );

private slots:
	//! Set grid step.
	void slotSetGridStep();
	//! Resize form.
	void resizeForm();
	//! Properties.
	void properties();

protected:
	void contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
		Q_DECL_OVERRIDE;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
		Q_DECL_OVERRIDE;
	void mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent )
		Q_DECL_OVERRIDE;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
		Q_DECL_OVERRIDE;
	void hoverEnterEvent( QGraphicsSceneHoverEvent * event )
		Q_DECL_OVERRIDE;
	void hoverMoveEvent( QGraphicsSceneHoverEvent * event )
		Q_DECL_OVERRIDE;
	void dragEnterEvent( QGraphicsSceneDragDropEvent * event )
		Q_DECL_OVERRIDE;
	void dragMoveEvent( QGraphicsSceneDragDropEvent * event )
		Q_DECL_OVERRIDE;
	void dropEvent( QGraphicsSceneDragDropEvent * event )
		Q_DECL_OVERRIDE;

protected:
	friend class UndoAddLineToPoly;

	//! Set current line.
	void setCurrentLine( FormLine * line );
	//! Set current polyline.
	void setCurrentPolyLine( FormPolyline * line );

private:
	friend class FormPrivate;

	Q_DISABLE_COPY( Form )

	QScopedPointer< FormPrivate > d;
}; // class Form

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HPP__INCLUDED
