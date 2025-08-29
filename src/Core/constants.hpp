
/*
    SPDX-FileCopyrightText: 2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTOTYPER__CORE__CONSTANTS_HPP__INCLUDED
#define PROTOTYPER__CORE__CONSTANTS_HPP__INCLUDED

// Qt include.
#include <QColor>
#include <QtGlobal>

namespace Prototyper
{

namespace Core
{

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
static const QColor c_linkColor = QColor(33, 122, 255);

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__CONSTANTS_HPP__INCLUDED
