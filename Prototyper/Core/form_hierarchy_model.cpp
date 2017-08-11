
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

// Prototyper include.
#include "form_hierarchy_model.hpp"
#include "form_object.hpp"

// Qt include.
#include <QList>
#include <QGraphicsItem>
#include <QIcon>


namespace Prototyper {

namespace Core {

//
// FormHierarchyModelPrivate
//

class FormHierarchyModelPrivate {
public:
	explicit FormHierarchyModelPrivate( FormHierarchyModel * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();
	//! \return Child objects.
	QList< FormObject* > children( FormObject * parent ) const;
	//! \return Row number.
	int row( FormObject * obj ) const;

	//! Parent.
	FormHierarchyModel * q;
	//! Root items.
	QList< FormObject* > m_root;
}; // class FormHierarchyModelPrivate

void
FormHierarchyModelPrivate::init()
{
}

QList< FormObject* >
FormHierarchyModelPrivate::children( FormObject * parent ) const
{
	QList< FormObject* > res;

	QGraphicsItem * root = dynamic_cast< QGraphicsItem* > ( parent );

	if( root )
	{
		QList< QGraphicsItem* > items = root->childItems();

		foreach( QGraphicsItem * i, items )
		{
			FormObject * obj = dynamic_cast< FormObject* > ( i );

			if( obj )
				res.append( obj );
		}
	}

	return res;
}

int
FormHierarchyModelPrivate::row( FormObject * obj ) const
{
	if( m_root.contains( obj ) )
		return m_root.indexOf( obj );

	int res = -1;

	QGraphicsItem * item = dynamic_cast< QGraphicsItem* > ( obj );

	if( item )
	{
		FormObject * parent = dynamic_cast< FormObject* > ( item->parentItem() );

		if( parent )
		{
			QList< FormObject* > objs = children( parent );

			return objs.indexOf( obj );
		}
	}

	return res;
}


//
// FormHierarchyModel
//

FormHierarchyModel::FormHierarchyModel( QObject * parent )
	:	QAbstractItemModel( parent )
	,	d( new FormHierarchyModelPrivate( this ) )
{
	d->init();
}

FormHierarchyModel::~FormHierarchyModel()
{
}

void
FormHierarchyModel::addForm( FormObject * form )
{
	if( !d->m_root.contains( form ) )
	{
		beginInsertRows( QModelIndex(), d->m_root.size(), d->m_root.size() );
		d->m_root.append( form );
		endInsertRows();
	}
}

void
FormHierarchyModel::removeForm( FormObject * form )
{
	if( d->m_root.contains( form ) )
	{
		const int index = d->m_root.indexOf( form );

		beginRemoveRows( QModelIndex(), index, index );
		d->m_root.removeAt( index );
		endRemoveRows();
	}
}

void
FormHierarchyModel::renameForm( FormObject * form )
{
	if( d->m_root.contains( form ) )
	{
		const QModelIndex i = index( d->m_root.indexOf( form ), 0 );

		emit dataChanged( i, i );
	}
}

void
FormHierarchyModel::clear()
{
	beginResetModel();
	d->m_root.clear();
	endResetModel();
}

void
FormHierarchyModel::addObject( FormObject * obj, FormObject * parent )
{
	Q_UNUSED( obj )

	QModelIndex i = index( parent );

	if( i.isValid() )
	{
		const int row = rowCount( i );

		beginInsertRows( i, row, row );
		endInsertRows();
	}
}

void
FormHierarchyModel::removeObject( FormObject * obj, FormObject * parent )
{
	QModelIndex i = index( parent );

	if( i.isValid() )
	{
		const int r = d->row( obj );

		beginRemoveRows( i, r, r );
	}
}

void
FormHierarchyModel::endRemoveObject()
{
	endRemoveRows();
}

void
FormHierarchyModel::update( FormObject * obj )
{
	QModelIndex start = index( obj );
	QModelIndex end = createIndex( start.row(), 3, start.internalPointer() );

	emit dataChanged( start, end );
}

bool
FormHierarchyModel::isLinked( const QModelIndex & index ) const
{
	if( index.isValid() )
	{
		FormObject * obj = static_cast< FormObject* >
			( index.internalPointer() );

		return ( !obj->link().isEmpty() );
	}
	else
		return false;
}

QModelIndex
FormHierarchyModel::index( FormObject * obj ) const
{
	const int row = d->row( obj );

	if( row >= 0 )
		return createIndex( row, 0, obj );
	else
		return QModelIndex();
}

int
FormHierarchyModel::columnCount( const QModelIndex & parent ) const
{
	Q_UNUSED( parent )

	return 3;
}

static inline QString objectType( FormObject * obj )
{
	switch( obj->objectType() )
	{
		case FormObject::LineType :
		{
			return FormHierarchyModel::tr( "Line" );
		}
			break;

		case FormObject::PolylineType :
		{
			return FormHierarchyModel::tr( "Polyline" );
		}
			break;

		case FormObject::TextType :
		{
			return FormHierarchyModel::tr( "Text" );
		}
			break;

		case FormObject::ImageType :
		{
			return FormHierarchyModel::tr( "Image" );
		}
			break;

		case FormObject::RectType :
		{
			return FormHierarchyModel::tr( "Rect" );
		}
			break;

		case FormObject::GroupType :
		{
			return FormHierarchyModel::tr( "Group" );
		}
			break;

		case FormObject::FormType :
		{
			return FormHierarchyModel::tr( "Form" );
		}
			break;

		case FormObject::ButtonType :
		{
			return FormHierarchyModel::tr( "Button" );
		}
			break;

		case FormObject::ComboBoxType :
		{
			return FormHierarchyModel::tr( "ComboBox" );
		}
			break;

		case FormObject::RadioButtonType :
		{
			return FormHierarchyModel::tr( "Radio Button" );
		}
			break;

		case FormObject::CheckBoxType :
		{
			return FormHierarchyModel::tr( "CheckBox" );
		}
			break;

		case FormObject::HSliderType :
		{
			return FormHierarchyModel::tr( "Horizontal Slider" );
		}
			break;

		case FormObject::VSliderType :
		{
			return FormHierarchyModel::tr( "Vertical Slider" );
		}
			break;

		case FormObject::SpinBoxType :
		{
			return FormHierarchyModel::tr( "SpinBox" );
		}
			break;

		default :
			return QString();
	}
}

QVariant
FormHierarchyModel::data( const QModelIndex & index, int role ) const
{
	Q_UNUSED( index )

	switch( role )
	{
		case Qt::DisplayRole :
		case Qt::EditRole :
		{
			switch( index.column() )
			{
				case 0 :
				{
					FormObject * obj = static_cast< FormObject* >
						( index.internalPointer() );

					return obj->objectId();
				}
					break;

				case 1 :
				{
					FormObject * obj = static_cast< FormObject* >
						( index.internalPointer() );

					return objectType( obj );
				}
					break;

				case 2 :
				{
					FormObject * obj = static_cast< FormObject* >
						( index.internalPointer() );

					return obj->link();
				}
					break;

				default :
					return QVariant();
			}
		}
			break;

		case Qt::DecorationRole :
		{
			if( index.column() == 0 )
			{
				FormObject * obj = static_cast< FormObject* >
					( index.internalPointer() );

				if( !obj->link().isEmpty() )
					return QIcon( ":/Core/img/link.png" );
				else
					return QVariant();
			}
			else
				return QVariant();
		}
			break;

		default :
			return QVariant();
	}
}

QModelIndex
FormHierarchyModel::index( int row, int column,
	const QModelIndex & parent) const
{
	if( parent.isValid() )
	{
		FormObject * obj = static_cast< FormObject* >
			( parent.internalPointer() );

		QList< FormObject* > objs = d->children( obj );

		if( row < objs.size() )
			return createIndex( row, column, objs.at( row ) );
		else
			return QModelIndex();
	}
	else if( row < d->m_root.size() )
		return createIndex( row, column, d->m_root.at( row ) );
	else
		return QModelIndex();
}

QModelIndex
FormHierarchyModel::parent( const QModelIndex & index ) const
{
	FormObject * obj = static_cast< FormObject* > ( index.internalPointer() );

	QGraphicsItem * item = dynamic_cast< QGraphicsItem* >
		( obj );

	if( item )
	{
		FormObject * p = dynamic_cast< FormObject* >
			( item->parentItem() );

		if( p )
		{
			const int row = d->row( p );

			if( row >= 0 )
				return createIndex( row, 0, p );
			else
				return QModelIndex();
		}
		else
			return QModelIndex();
	}
	else
		return QModelIndex();
}

int
FormHierarchyModel::rowCount( const QModelIndex & parent ) const
{
	if( parent.isValid() )
	{
		FormObject * obj = static_cast< FormObject* >
			( parent.internalPointer() );

		if( obj )
			return d->children( obj ).size();
		else
			return 0;
	}
	else
		return d->m_root.size();
}

QVariant
FormHierarchyModel::headerData( int section, Qt::Orientation orientation,
	int role ) const
{
	if( orientation == Qt::Vertical )
		return QVariant();

	if( role != Qt::DisplayRole )
		return QVariant();

	switch( section )
	{
		case 0 :
			return tr( "Name" );

		case 1 :
			return tr( "Type" );

		case 2 :
			return tr( "Linked To" );

		default :
			return QVariant();
	}
}

} /* namespace Core */

} /* namespace Prototyper */
