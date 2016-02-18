
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
#include "pdf_exporter.hpp"


namespace Prototyper {

namespace Core {

//
// PdfExporterPrivate
//

class PdfExporterPrivate
	:	public ExporterPrivate
{
public:
	PdfExporterPrivate( const Cfg::Project & cfg, PdfExporter * parent )
		:	ExporterPrivate( cfg, parent )
	{
	}

	//! Init.
	void init() Q_DECL_OVERRIDE;
}; // class PdfExporterPrivate

void
PdfExporterPrivate::init()
{
	ExporterPrivate::init();
}


//
// PdfExporter
//

PdfExporter::PdfExporter( const Cfg::Project & project )
	:	Exporter( QScopedPointer< ExporterPrivate >
			( new PdfExporterPrivate( project, this ) ) )
{
}

PdfExporter::~PdfExporter()
{
}

void
PdfExporter::exportToDoc( const QString & fileName ) const
{
	Q_UNUSED( fileName )
}

} /* namespace Core */

} /* namespace Prototyper */
