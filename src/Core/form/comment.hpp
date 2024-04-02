
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER_CORE_FORM_COMMENT_HPP_INCLUDED
#define PROTOTYPER_CORE_FORM_COMMENT_HPP_INCLUDED

// Qt include.
#include <QGraphicsObject>

// C++ include.
#include <memory>

// Prototyper include.
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

//
// PageComment
//

class PageCommentPrivate;

//! Comment on the page.
class PageComment final
	:	public QGraphicsObject
{
	Q_OBJECT

signals:
	//! Show comments.
	void showComments();
	//! Comments changed.
	void changed();

public:
	PageComment( QGraphicsItem * parent = nullptr );
	~PageComment() override;

	//! \return Cfg.
	Cfg::Comments cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Comments & c );

	//! \return Author.
	QString author() const;
	//! Set author.
	void setAuthor( const QString & name );

	QRectF boundingRect() const override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

	//! Set ID of the comments.
	void setId( int id );
	//! \return ID.
	int id() const;

	//! Is this comment under point.
	bool isItYou( const QPointF & point ) const;

	//! \return Is changed?
	bool isChanged() const;
	//! Set changed flag.
	void setChanged( bool on = true );

private slots:
	void showCommentsImpl();

protected:
	void mousePressEvent( QGraphicsSceneMouseEvent * e ) override;
	void mouseMoveEvent( QGraphicsSceneMouseEvent * e ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * e ) override;

private:
	Q_DISABLE_COPY( PageComment )

	std::unique_ptr< PageCommentPrivate > d;
}; // class PageComment

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER_CORE_FORM_COMMENT_HPP_INCLUDED
