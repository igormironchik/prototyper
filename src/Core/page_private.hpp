
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

#ifndef PROTOTYPER__CORE__PAGE_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__PAGE_PRIVATE_HPP__INCLUDED

// Qt include.
#include <QScopedPointer>
#include <QList>
#include <QMap>
#include <QPointF>

// C++ include.
#include <vector>

// Prototyper include.
#include "types.hpp"


QT_BEGIN_NAMESPACE
class QUndoStack;
class QAction;
class QGraphicsItem;

template< typename >
class QSharedPointer;

class QTextDocument;
class QRectF;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

namespace Cfg {

class Page;
class Size;
class Text;
class Group;
class TextStyle;

} /* namespace Cfg */

class Page;
class FormLine;
class FormText;
class FormGroup;
class GridSnap;
class FormPolyline;
class PageComment;


//
// PagePrivate
//

class PagePrivate {
public:
	PagePrivate( Cfg::Page & cfg, Page * parent )
		:	q( parent )
		,	m_gridMode( ShowGrid )
		,	m_gridStepAction( 0 )
		,	m_cfg( cfg )
		,	m_pressed( false )
		,	m_current( 0 )
		,	m_id( 0 )
		,	m_snap( 0 )
		,	m_polyline( false )
		,	m_isCommentChanged( false )
		,	m_currentPoly( 0 )
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
	void ungroup( QGraphicsItem * group, bool pushUndoCommand = true );
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
	//! Set text.
	void setText( const QSharedPointer< QTextDocument > & doc,
		const std::vector< Cfg::TextStyle > & text );
	//! Hide handles of current item.
	void hideHandlesOfCurrent();
	//! Selection.
	QList< QGraphicsItem* > selection();
	//! Is comment under mosue?
	bool isCommentUnderMouse() const;

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
	Page * q;
	//! Grid mode.
	GridMode m_gridMode;
	//! Grid step action.
	QAction * m_gridStepAction;
	//! Cfg.
	Cfg::Page & m_cfg;
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
	//! Comment was added/deleted;
	bool m_isCommentChanged;
	//! Current polyline.
	FormPolyline * m_currentPoly;
	//! IDs
	QStringList m_ids;
	//! Undo stack.
	QUndoStack * m_undoStack;
	//! Map of text documents.
	QMap< QObject*, FormText* > m_docs;
	//! Comments.
	QList< PageComment* > m_comments;
}; // class PagePrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PAGE_PRIVATE_HPP__INCLUDED
