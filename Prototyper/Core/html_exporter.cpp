
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
#include "html_exporter.hpp"

// Qt include.
#include <QSvgGenerator>
#include <QPainter>


namespace Prototyper {

namespace Core {

//
// HtmlExporterPrivate
//

class HtmlExporterPrivate
	:	public ExporterPrivate
{
public:
	HtmlExporterPrivate( const Cfg::Project & cfg, HtmlExporter * parent )
		:	ExporterPrivate( cfg, parent )
	{
	}

	//! Init.
	void init() Q_DECL_OVERRIDE;
}; // class HtmlExporterPrivate

void
HtmlExporterPrivate::init()
{
	ExporterPrivate::init();
}


//
// HtmlExporter
//

HtmlExporter::HtmlExporter( const Cfg::Project & project )
	:	Exporter( QScopedPointer< ExporterPrivate >
			( new HtmlExporterPrivate( project, this ) ) )
{
}

HtmlExporter::~HtmlExporter()
{
}

void
HtmlExporter::exportToDoc( const QString & fileName )
{
	Q_UNUSED( fileName )
}

} /* namespace Core */

} /* namespace Prototyper */
