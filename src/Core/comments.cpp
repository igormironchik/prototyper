
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

// Prototyper include.
#include "comments.hpp"
#include "ui_comments.h"

// Qt include.
#include <QPushButton>
#include <QStringListModel>
#include <QTextEdit>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QPainter>

#include <QDebug>


namespace Prototyper {

namespace Core {

//
// WordWrapItemDelegate
//

//! Item delegate for word wrapping.
class WordWrapItemDelegate final
	:	public QStyledItemDelegate
{
public:
	WordWrapItemDelegate( QTreeView * parent = nullptr )
		:	QStyledItemDelegate( parent )
		,	m_parent( parent )
	{
	}

	QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const override
	{
		const auto r = option.fontMetrics.boundingRect(
			QRect( 0, 0, m_parent->header()->sectionSize( index.column() ), 0 ),
			Qt::AlignLeft | Qt::TextWordWrap,
			index.data( Qt::DisplayRole ).toString() );

		return r.adjusted( 0, 0, 0, option.fontMetrics.height() ).size();
	}

private:
	QTreeView * m_parent;
}; // class WordWrapItemDelegate


//
// CommentsPrivate
//

class CommentsPrivate {
public:
	CommentsPrivate( Comments * parent )
		:	q( parent )
		,	m_model( nullptr )
		,	m_delegate( nullptr )
	{
	}

	//! Init.
	void init();

	//! Parent.
	Comments * q;
	//! Ui.
	Ui::Comments m_ui;
	//! Model.
	QStringListModel * m_model;
	//! Current index.
	QModelIndex m_currentIndex;
	//! Delegate.
	WordWrapItemDelegate * m_delegate;
}; // class CommentsPrivate

void
CommentsPrivate::init()
{
	m_ui.setupUi( q );

	m_model = new QStringListModel( q );
	m_ui.m_view->setModel( m_model );

	m_delegate = new WordWrapItemDelegate( m_ui.m_view );
	m_ui.m_view->setItemDelegate( m_delegate );

	m_ui.m_text->setFocus();

	Comments::connect( m_ui.m_ok, &QPushButton::clicked,
		q, &Comments::commit );
	Comments::connect( m_ui.m_cancel, &QPushButton::clicked,
		q, &Comments::cancel );
	Comments::connect( m_ui.m_view, &QTreeView::customContextMenuRequested,
		q, &Comments::contextMenuRequested );
	Comments::connect( m_ui.m_view->header(), &QHeaderView::sectionResized,
		q, &Comments::sectionResized );
}


//
// Comments
//

Comments::Comments( const QStringList & data, QWidget * parent )
	:	QDialog( parent )
	,	d( new CommentsPrivate( this ) )
{
	d->init();

	for( const auto & c : qAsConst( data ) )
		addComment( c );
}

Comments::~Comments() = default;

void
Comments::addComment( const QString & c )
{
	if( !c.isEmpty() )
	{
		if( !d->m_currentIndex.isValid() )
		{
			d->m_model->insertRow( d->m_model->rowCount() );
			d->m_model->setData( d->m_model->index( d->m_model->rowCount() - 1, 0 ),
				c, Qt::DisplayRole );
		}
		else
			d->m_model->setData( d->m_currentIndex, c, Qt::DisplayRole );
	}
}

QStringList
Comments::comments() const
{
	QStringList c;

	for( int i = 0; i < d->m_model->rowCount(); ++i )
		c.append( d->m_model->data( d->m_model->index( i, 0 ) ).toString() );

	return c;
}

void
Comments::commit()
{
	const auto text = d->m_ui.m_text->toPlainText().trimmed();

	addComment( text );

	d->m_ui.m_text->clear();
	d->m_currentIndex = QModelIndex();
	d->m_ui.m_text->setFocus();
}

void
Comments::cancel()
{
	d->m_currentIndex = QModelIndex();
	d->m_ui.m_text->clear();
	d->m_ui.m_text->setFocus();
}

void
Comments::contextMenuRequested( const QPoint & pos )
{
	if( d->m_ui.m_view->indexAt( pos).isValid() )
	{
		QMenu menu;
		menu.addAction( QIcon( QStringLiteral( ":/Core/img/document-edit.png" ) ),
			tr( "Edit" ),
			[&]()
			{
				d->m_currentIndex = d->m_ui.m_view->indexAt( pos );
				d->m_ui.m_text->setPlainText(
					d->m_currentIndex.data( Qt::DisplayRole ).toString() );
				d->m_ui.m_text->setFocus();
			} );
		menu.addAction( QIcon( QStringLiteral( ":/Core/img/edit-delete.png" ) ),
			tr( "Delete" ),
			[&]()
			{
				d->m_model->removeRow( d->m_ui.m_view->indexAt( pos ).row() );
				d->m_ui.m_text->setFocus();
			} );

		menu.exec( mapToGlobal( pos ) );
	}
}

void
Comments::sectionResized( int section, int, int )
{
	for( int i = 0; i < d->m_model->rowCount(); ++i )
		emit d->m_delegate->sizeHintChanged( d->m_model->index( i, section ) );
}

} /* namespace Core */

} /* namespace Prototyper */
