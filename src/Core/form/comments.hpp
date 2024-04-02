
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
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
	explicit Comments( const QString & author,
		const QVector< QPair< QString, QString > > & data,
		QWidget * parent = nullptr );
	~Comments() override;

	//! \return List of comments.
	QVector< QPair< QString, QString > > comments() const;

	//! \return Author.
	QString author() const;

private slots:
	//! Write comment.
	void commit();
	//! Cancel comment.
	void cancel();
	//! Context menu.
	void contextMenuRequested( const QPoint & pos );
	//! Section resized.
	void sectionResized( int logicalIndex, int oldSize, int newSize );
	//! Set buttons state.
	void setButtonsState();

private:
	//! Add new comment.
	void addComment( const QPair< QString, QString > & c );

private:
	friend class CommentsPrivate;

	Q_DISABLE_COPY( Comments )

	std::unique_ptr< CommentsPrivate > d;
}; // class Comments

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER_COMMENTS_HPP_INCLUDED
