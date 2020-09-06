
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

#ifndef PROTOTYPER__CORE__CONSTANTS_HPP__INCLUDED
#define PROTOTYPER__CORE__CONSTANTS_HPP__INCLUDED

// Qt include.
#include <QtGlobal>
#include <QColor>


namespace Prototyper {

namespace Core {

static const qreal c_defaultFontSize = 10.0;
static const qreal c_halfDivider = 2.0;
static const int c_snapHalfSize = 5;
static const qreal c_mostTopZValue = 99999999999.9;
static const qreal c_halfHandleSize = 3.0;
static const qreal c_minResizableSize = 8.0;
static const qreal c_halfResizeHandleSize = 6.0;
static const qreal c_maxZero = 0.001;
static const int c_resolution = 150;
static const qreal c_zDelta = 0.5;
static const qreal c_maxDistNoMove = 5.0;
static const qreal c_zoomFactor = 0.05;
static const qreal c_minZoom = 0.1;
static const qreal c_maxZoom = 4.0;
static const qreal c_ensureVisibleRegion = 10.0;
static const qreal c_mmInInch = 25.4;
static const qreal c_ptInInch = 72.0;
static const qreal c_a4Width = 210.0;
static const qreal c_a4Height = 297.0;
static const qreal c_linePenWidth = 2.0;
static const qreal c_headerFontSize = 20.0;

static const QColor c_textColor = Qt::black;
static const QColor c_linkColor = QColor( 33, 122, 255 );

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__CONSTANTS_HPP__INCLUDED
