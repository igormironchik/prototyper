
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
#include "form_hierarchy_model.hpp"
#include "form_grid_snap.hpp"

QT_BEGIN_NAMESPACE
class QUndoStack;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

namespace Cfg {

class Form;
class Size;

} /* namespace Cfg */

//
// GridMode
//

//! Grid mode.
enum GridMode {
	//! Show grid.
	ShowGrid,
	//! No grid.
	NoGrid
}; // enum GridMode


//
// FormPrivate
//

class FormPrivate {
public:
	FormPrivate( Cfg::Form & cfg, Form * parent )
		:	q( parent )
		,	m_gridMode( ShowGrid )
		,	m_gridStepAction( 0 )
		,	m_cfg( cfg )
		,	m_pressed( false )
		,	m_current( 0 )
		,	m_id( 0 )
		,	m_snap( 0 )
		,	m_polyline( false )
		,	m_currentPoly( 0 )
		,	m_model( 0 )
		,	m_btns( FormProperties::MinimizeButton |
				FormProperties::MaximizeButton |
				FormProperties::CloseButton )
		,	m_undoStack( 0 )
	{
	}

	//! Init.
	void init();
	//! \return Current Z-value.
	qreal currentZValue() const;
	//! \return Current Z-value.
	void currentZValue( const QList< QGraphicsItem* > & items,
		qreal & z ) const;
	//! \return Start point for line.
	QPointF lineStartPoint( const QPointF & point,
		bool & intersected, bool & intersectedEnds,
		FormLine* & intersectedLine ) const;
	//! Clear current lines.
	void clearCurrentLines();
	//! Handle mouse move in current lines.
	void handleMouseMoveInCurrentLines( const QPointF & point );
	//! Handle mouse move in current polyline.
	void handleMouseMoveInCurrentPolyLine( const QPointF & point );
	//! Ungroup.
	void ungroup( QGraphicsItem * group );
	//! \return Next ID.
	QString id();
	//! Update form from the configuration.
	void updateFromCfg();
	//! Clear form.
	void clear();
	//! Create text.
	FormText * createText( const Cfg::Text & cfg );
	//! Create group.
	FormGroup * createGroup( const Cfg::Group & cfg );
	//! Create element.
	template< class Elem, class Config >
	Elem * createElem( const Config & cfg );
	//! Create element with rect.
	template< class Elem, class Config >
	Elem * createElemWithRect( const Config & cfg, const QRectF & rect );
	//! Clear IDs.
	void clearIds( FormGroup * group );
	//! Add IDs.
	void addIds( FormGroup * group );
	//! Create description.
	void createDescription( const QString & id );
	//! Set text.
	void setText( const QSharedPointer< QTextDocument > & doc,
		const QList< Cfg::TextStyle > & text );
	//! Update link.
	void updateLink( const QList< QGraphicsItem* > & childItems,
		const QString & oldName, const QString & name );
	//! Hide handles of current item.
	void hideHandlesOfCurrent();
	//! Remove descriptions of the object.
	void removeDescriptions( FormObject * obj );
	//! Selection.
	QList< QGraphicsItem* > selection();

	//! AlignPoint.
	enum AlignPoint {
		//! Align Horizontal Left.
		AlignHorLeftPoint,
		//! Align Horizontal Center.
		AlignHorCenterPoint,
		//! Align Horizontal Right.
		AlignHorRightPoint,
		//! Align Vertical Top.
		AlignVertTopPoint,
		//! Align Vertical Center.
		AlignVertCenterPoint,
		//! Align Vertical Bottom.
		AlignVertBottomPoint
	}; // enum AlignPoint

	//! Search align point.
	qreal searchAlignPoint( const QList< QGraphicsItem* > & items,
		AlignPoint point );

	//! Parent.
	Form * q;
	//! Grid mode.
	GridMode m_gridMode;
	//! Grid step action.
	QAction * m_gridStepAction;
	//! Cfg.
	Cfg::Form & m_cfg;
	//! Pressed.
	bool m_pressed;
	//! Current item.
	QGraphicsItem * m_current;
	//! Mouse pos.
	QPointF m_pos;
	//! ID.
	quint64 m_id;
	//! Current lines.
	QList< FormLine* > m_currentLines;
	//! Grid snap.
	GridSnap * m_snap;
	//! Make polyline.
	bool m_polyline;
	//! Current polyline.
	FormPolyline * m_currentPoly;
	//! Hierarchy model.
	FormHierarchyModel * m_model;
	//! IDs
	QStringList m_ids;
	//! Descriptions.
	QMap< QString, QSharedPointer< QTextDocument > > m_desc;
	//! Buttons.
	FormProperties::Buttons m_btns;
	//! Undo stack.
	QUndoStack * m_undoStack;
}; // class FormPrivate


//
// Form
//

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
	void deleteItems( const QList< QGraphicsItem* > & items,
		bool makeUndoCommand = true );

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
	template< class Elem, class Config > friend class UndoCreate;
	template< class Elem, class Config > friend class UndoDelete;

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

		d->m_ids.append( id );

		d->m_model->addObject( obj, this );

		return obj;
	}

private:
	friend class FormPrivate;

	Q_DISABLE_COPY( Form )

	QScopedPointer< FormPrivate > d;
}; // class Form

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HPP__INCLUDED
