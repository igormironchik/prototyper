
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

#ifndef PROTOTYPER_COMMENTS_HPP_INCLUDED
#define PROTOTYPER_COMMENTS_HPP_INCLUDED

// Qt include.
#include <QDialog>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

//
// Comments
//

class CommentsPrivate;

//! Widget with comments.
class Comments final
	:	public QDialog
{
	Q_OBJECT

public:
	explicit Comments( const QStringList & data, QWidget * parent = nullptr );
	~Comments() override;

	//! \return List of comments.
	QStringList comments() const;

private slots:
	//! Write comment.
	void commit();
	//! Cancel comment.
	void cancel();
	//! Context menu.
	void contextMenuRequested( const QPoint & pos );
	//! Section resized.
	void sectionResized( int logicalIndex, int oldSize, int newSize );

private:
	//! Add new comment.
	void addComment( const QString & c );

private:
	friend class CommentsPrivate;

	Q_DISABLE_COPY( Comments )

	std::unique_ptr< CommentsPrivate > d;
}; // class Comments

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER_COMMENTS_HPP_INCLUDED
