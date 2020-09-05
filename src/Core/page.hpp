
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

#ifndef PROTOTYPER__CORE__FORM_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HPP__INCLUDED

// Qt include.
#include <QGraphicsObject>
#include <QScopedPointer>
#include <QTextDocument>

// Prototyper include.
#include "types.hpp"
#include "form_object.hpp"
#include "page_private.hpp"
#include "form_line.hpp"
#include "form_polyline.hpp"
#include "form_group.hpp"
#include "form_text.hpp"
#include "form_image.hpp"
#include "form_rectangle.hpp"
#include "form_button.hpp"
#include "form_checkbox.hpp"
#include "form_radio_button.hpp"
#include "form_combobox.hpp"
#include "form_spinbox.hpp"
#include "form_hslider.hpp"
#include "form_vslider.hpp"


QT_BEGIN_NAMESPACE
class QUndoStack;
class QAction;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

namespace Cfg {

class Form;
class Size;
class Page;

} /* namespace Cfg */

class PagePrivate;
class FormLine;
class FormPolyline;
class FormText;
class FormGroup;
class GridSnap;


//
// Page
//

//! Page.
class Page
	:	public QGraphicsObject
	,	public FormObject
{
	Q_OBJECT

signals:
	//! Changed.
	void changed();

public:
	explicit Page( Cfg::Page & c, QGraphicsItem * parent = 0 );
	~Page();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return PageType; }

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
	Cfg::Page cfg() const;
	//! Set configuration.
	void setCfg( const Cfg::Page & c );

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
	FormGroup * group( const QList< QGraphicsItem* > & items,
		bool pushUndoCommand = true, const QString & id = QString() );
	//! Ungroup group.
	void ungroup( FormGroup * g, bool pushUndoCommand = true );

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
	void deleteItems( const QList< QGraphicsItem* > & items,
		bool makeUndoCommand = true );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget ) Q_DECL_OVERRIDE;

	static void draw( QPainter * painter, int width, int height,
		int gridStep, bool drawGrid = true );

	//! Position elements.
	void setPosition( const QPointF & pos,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

	//! \return Position of the element.
	QPointF position() const Q_DECL_OVERRIDE;

	//! \return Rectangle of the element.
	QRectF rectangle() const Q_DECL_OVERRIDE;

	//! Set rectangle.
	void setRectangle( const QRectF & rect,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

	//! Emit changed signal.
	void emitChanged();

	//! Next id.
	QString nextId();

	//! Clone object.
	FormObject * clone() const override;

	//! \return Is comment changed?
	bool isCommentChanged() const;
	//! Clear comment changed flag.
	void clearCommentChanged();

	//! \return Max Z index on the page.
	qreal topZ() const;
	//! \return Min Z index on the page.
	qreal bottomZ() const;

public slots:
	//! Rename form.
	void renameForm( const QString & name );
	//! Clear edit mode in texts.
	void clearEditModeInTexts();

private slots:
	//! Set grid step.
	void slotSetGridStep();
	//! Undo command in text item was added.
	void undoCommandInTextAdded();

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
	template< class Elem, class Config > friend class UndoCreate;
	template< class Elem, class Config > friend class UndoDelete;
	friend class UndoChangeTextOnForm;

	//! Remove document from map.
	void removeDocFromMap( QObject * doc );
	//! Update document's item.
	void updateDocItemInMap( QObject * doc, FormText * text );
	//! Set current line.
	void setCurrentLine( FormLine * line );
	//! Set current polyline.
	void setCurrentPolyLine( FormPolyline * line );
	//! Create element.
	template< class Elem >
	FormObject * createElement( const QString & id )
	{
		FormObject * obj = Q_NULLPTR;

		switch( Elem::staticObjectType() )
		{
			case FormObject::LineType :
			{
				obj = new FormLine( this, this );
			}
				break;

			case FormObject::PolylineType :
			{
				obj = new FormPolyline( this, this );
			}
				break;

			case FormObject::TextType :
			{
				obj = new FormText( QRectF(), this, this );
			}
				break;

			case FormObject::ImageType :
			{
				obj = new FormImage( this, this );
			}
				break;

			case FormObject::RectType :
			{
				obj = new FormRect( this, this );
			}
				break;

			case FormObject::GroupType :
			{
				obj = new FormGroup( this, this );
			}
				break;

			case FormObject::ButtonType :
			{
				obj = new FormButton( QRectF(), this, this );
			}
				break;

			case FormObject::ComboBoxType :
			{
				obj = new FormComboBox( QRectF(), this, this );
			}
				break;

			case FormObject::RadioButtonType :
			{
				obj = new FormRadioButton( QRectF(), this, this );
			}
				break;

			case FormObject::CheckBoxType :
			{
				obj = new FormCheckBox( QRectF(), this, this );
			}
				break;

			case FormObject::HSliderType :
			{
				obj = new FormHSlider( QRectF(), this, this );
			}
				break;

			case FormObject::VSliderType :
			{
				obj = new FormVSlider( QRectF(), this, this );
			}
				break;

			case FormObject::SpinBoxType :
			{
				obj = new FormSpinBox( QRectF(), this, this );
			}
				break;

			default :
				break;
		}

		obj->setObjectId( id );

		dynamic_cast< QGraphicsItem* > ( obj )->setZValue( d->currentZValue() + 1.0 );

		d->m_ids.append( id );

		return obj;
	}

private:
	friend class PagePrivate;

	Q_DISABLE_COPY( Page )

	QScopedPointer< PagePrivate > d;
}; // class Page

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HPP__INCLUDED
