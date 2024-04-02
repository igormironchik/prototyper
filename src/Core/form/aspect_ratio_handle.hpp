
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__FORM_ASPECT_RATIO_HANDLE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_ASPECT_RATIO_HANDLE_HPP__INCLUDED

// Qt include.
#include <QGraphicsItem>

// C++ include.
#include <memory>


namespace Prototyper {

namespace Core {

//
// AspectRatioHandle
//

class AspectRatioHandlePrivate;

//! Aspect ratio handle.
class AspectRatioHandle final
	:	public QGraphicsItem
{
public:
	explicit AspectRatioHandle( QGraphicsItem * parent );
	~AspectRatioHandle() override;

	//! \return Is keep aspect ratio?
	bool isKeepAspectRatio() const;
	//! Set keep aspect ratio.
	void setKeepAspectRatio( bool on = true );

	QRectF boundingRect() const override;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) override;

protected:
	void hoverEnterEvent( QGraphicsSceneHoverEvent * event ) override;
	void hoverLeaveEvent( QGraphicsSceneHoverEvent * event ) override;
	void mousePressEvent( QGraphicsSceneMouseEvent * event ) override;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) override;

private:
	Q_DISABLE_COPY( AspectRatioHandle )

	std::unique_ptr< AspectRatioHandlePrivate > d;
}; // class AspectRatioHandle

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_ASPECT_RATIO_HANDLE_HPP__INCLUDED
