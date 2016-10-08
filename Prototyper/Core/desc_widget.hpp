
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

#ifndef PROTOTYPER__CORE__DESC_WIDGET_HPP__INCLUDED
#define PROTOTYPER__CORE__DESC_WIDGET_HPP__INCLUDED

// Qt include.
#include <QDockWidget>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

//
// DescDockWidget
//

class DescDockWidgetPrivate;

//! Dock widget with description.
class DescDockWidget Q_DECL_FINAL
	:	public QDockWidget
{
	Q_OBJECT

signals:
	//! Changed.
	void changed();

public:
	explicit DescDockWidget( QWidget * parent );
	~DescDockWidget();

	//! Set document.
	void setDocument( QSharedPointer< QTextDocument > doc,
		const QString & id );

private:
	Q_DISABLE_COPY( DescDockWidget )

	QScopedPointer< DescDockWidgetPrivate > d;
}; // class DescDockWidget

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__DESC_WIDGET_HPP__INCLUDED
