
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

#ifndef PROTOTYPER__CORE__UTILS_HPP__INCLUDED
#define PROTOTYPER__CORE__UTILS_HPP__INCLUDED

// Qt include.
#include <QtGlobal>

// Prototyper include.
#include "project_cfg.hpp"


QT_BEGIN_NAMESPACE
class QTextCharFormat;
class QTextCursor;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

//
// operator != for QTextCharFormat
//

bool operator != ( const QTextCharFormat & f1, const QTextCharFormat & f2 );


namespace Cfg {

static const QString c_boldStyle = QLatin1String( "bold" );
static const QString c_italicStyle = QLatin1String( "italic" );
static const QString c_underlineStyle = QLatin1String( "underline" );
static const QString c_normalStyle = QLatin1String( "normal" );


//
// textStyle
//

//! \return Text style for the configuration.
QList< QString > textStyle( const QTextCharFormat & f );


//
// text
//

//! \return Text for the configuration.
QList< Cfg::TextStyle > text( QTextCursor c, const QString & data );

} /* namespace Cfg */

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__UTILS_HPP__INCLUDED
