
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

// Prototyper include.
#include "comments.hpp"
#include "ui_comments.h"

// Qt include.
#include <QPushButton>
#include <QTextEdit>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QLineEdit>
#include <QAbstractListModel>
#include <QStyleOptionViewItem>


namespace Prototyper {

namespace Core {

//
// ListModel
//

class ListModel final
	:	public QAbstractListModel
{
public:
	explicit ListModel( QObject * parent )
		:	QAbstractListModel( parent )
	{
	}

	~ListModel() override = default;

	int rowCount( const QModelIndex & parent = QModelIndex() ) const override
	{
		if( parent.isValid() )
			return 0;

		return m_data.size();
	}

	QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const override
	{
		if( !index.isValid() )
			return {};

		switch( role )
		{
			case Qt::DisplayRole :
				return m_data.at( index.row() ).second;

			case Qt::UserRole :
				return m_data.at( index.row() ).first;

			default :
				return {};
		}
	}

    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole ) override
	{
		if( !index.isValid() )
			return false;

		if( role != Qt::DisplayRole && role != Qt::UserRole )
			return false;

		switch( role )
		{
			case Qt::DisplayRole :
				m_data[ index.row() ].second = value.toString();
				break;

			case Qt::UserRole :
				m_data[ index.row() ].first = value.toString();
				break;

			default :
				return false;
		}

		emit dataChanged( index, index, { Qt::DisplayRole, Qt::UserRole } );

		return true;
	}

	bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() ) override
	{
		if( parent.isValid() )
			return false;

		if( count < 1 || row < 0 || row > rowCount( parent ) )
			return false;

		beginInsertRows( parent, row, row + count - 1 );

		for (int r = 0; r < count; ++r)
			m_data.insert( row, qMakePair( QString(), QString() ) );

		endInsertRows();

		return true;
	}

    bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() ) override
	{
		if( parent.isValid() )
			return false;

		if( count <= 0 || row < 0 || ( row + count ) > rowCount( parent ) )
			return false;

		beginRemoveRows( QModelIndex(), row, row + count - 1 );

		const auto it = m_data.begin() + row;
		m_data.erase( it, it + count );

		endRemoveRows();

		return true;
	}

private:
	QVector< QPair< QString, QString > > m_data;
}; // class ListModel


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

	void paint( QPainter * painter, const QStyleOptionViewItem & option,
		const QModelIndex & index ) const override
	{
		auto opts = option;
		opts.displayAlignment = Qt::AlignLeft | Qt::AlignTop;

		QStyledItemDelegate::paint( painter, opts, index );

		QTextOption to;
		to.setAlignment( Qt::AlignRight | Qt::AlignBottom );

		QFont f = painter->font();

		if( f.pixelSize() > 0 )
			f.setPixelSize( f.pixelSize() * 0.75 );
		else
			f.setPointSize( f.pointSize() * 0.75 );

		f.setItalic( true );

		painter->setFont( f );

		painter->drawText( opts.rect.adjusted( 0, 0, -opts.fontMetrics.averageCharWidth(), 0 ),
			index.data( Qt::UserRole ).toString(), to );
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
	void init( const QString & author );

	//! Parent.
	Comments * q;
	//! Ui.
	Ui::Comments m_ui;
	//! Model.
	ListModel * m_model;
	//! Current index.
	QModelIndex m_currentIndex;
	//! Delegate.
	WordWrapItemDelegate * m_delegate;
}; // class CommentsPrivate

void
CommentsPrivate::init( const QString & author )
{
	m_ui.setupUi( q );

	m_model = new ListModel( q );
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
	Comments::connect( m_ui.m_name, &QLineEdit::textChanged,
		q, &Comments::nameChanged );

	m_ui.m_name->setText( author );
}


//
// Comments
//

Comments::Comments( const QString & author,
	const QVector< QPair< QString, QString > > & data, QWidget * parent )
	:	QDialog( parent )
	,	d( new CommentsPrivate( this ) )
{
	d->init( author );

	for( const auto & c : qAsConst( data ) )
		addComment( c );
}

Comments::~Comments() = default;

void
Comments::addComment( const QPair< QString, QString > & c )
{
	if( !c.second.isEmpty() && !d->m_ui.m_name->text().isEmpty() )
	{
		if( !d->m_currentIndex.isValid() )
		{
			d->m_model->insertRow( d->m_model->rowCount() );
			d->m_model->setData( d->m_model->index( d->m_model->rowCount() - 1, 0 ),
				c.second, Qt::DisplayRole );
			d->m_model->setData( d->m_model->index( d->m_model->rowCount() - 1, 0 ),
				c.first, Qt::UserRole );
		}
		else
		{
			d->m_model->setData( d->m_currentIndex, c.second, Qt::DisplayRole );
			d->m_model->setData( d->m_currentIndex, c.first, Qt::UserRole );
		}
	}
}

QVector< QPair< QString, QString > >
Comments::comments() const
{
	QVector< QPair< QString, QString > > c;

	for( int i = 0; i < d->m_model->rowCount(); ++i )
		c.append( qMakePair(
			d->m_model->data( d->m_model->index( i, 0 ), Qt::UserRole ).toString(),
			d->m_model->data( d->m_model->index( i, 0 ) ).toString() ) );

	return c;
}

QString
Comments::author() const
{
	return d->m_ui.m_name->text();
}

void
Comments::commit()
{
	if( !d->m_ui.m_name->text().isEmpty() )
	{
		const auto text = d->m_ui.m_text->toPlainText().trimmed();

		addComment( qMakePair( d->m_ui.m_name->text(), text ) );

		d->m_ui.m_text->clear();
		d->m_currentIndex = QModelIndex();
		d->m_ui.m_text->setFocus();
	}
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

void
Comments::nameChanged( const QString & name )
{
	d->m_ui.m_ok->setEnabled( !name.isEmpty() );
}

} /* namespace Core */

} /* namespace Prototyper */
