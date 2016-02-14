
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

#ifndef PROTOTYPER__CORE__FORM_HIERARCHY_MODEL_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_HIERARCHY_MODEL_HPP__INCLUDED

// Qt include.
#include <QAbstractItemModel>
#include <QScopedPointer>


namespace Prototyper {

namespace Core {

class FormObject;

//
// FormHierarchyModel
//

class FormHierarchyModelPrivate;

//! Model with hierarchy of objects on the form.
class FormHierarchyModel
	:	public QAbstractItemModel
{
public:
	FormHierarchyModel( QObject * parent = 0 );
	~FormHierarchyModel();

	//! Add form.
	void addForm( FormObject * form );
	//! Remove form.
	void removeForm( FormObject * form );
	//! Rename form.
	void renameForm( FormObject * form );
	//! Clear model.
	void clear();

	//! Add object.
	void addObject( FormObject * obj, FormObject * parent );
	//! Remove object.
	void removeObject( FormObject * obj, FormObject * parent );
	//! End remove object.
	void endRemoveObject();

	//! \return Index for the given object.
	QModelIndex index( FormObject * obj ) const;

	int columnCount( const QModelIndex & parent = QModelIndex() ) const
		Q_DECL_OVERRIDE;

	QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const
		Q_DECL_OVERRIDE;

	QModelIndex index( int row, int column,
		const QModelIndex & parent = QModelIndex() ) const
			Q_DECL_OVERRIDE;

	QModelIndex parent( const QModelIndex & index ) const
		Q_DECL_OVERRIDE;

	int rowCount( const QModelIndex & parent = QModelIndex() ) const
		Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( FormHierarchyModel )

	QScopedPointer< FormHierarchyModelPrivate > d;
}; // class FormHierarchyModel

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_HIERARCHY_MODEL_HPP__INCLUDED
