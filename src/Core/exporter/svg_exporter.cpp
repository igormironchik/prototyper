
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016-2021 Igor Mironchik

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
#include "svg_exporter.hpp"
#include "../form/utils.hpp"
#include "exporter_private.hpp"
#include "../constants.hpp"

// Qt include.
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QPainter>
#include <QFile>


namespace Prototyper {

namespace Core {

//
// SvgExporterPrivate
//

class SvgExporterPrivate
	:	public ExporterPrivate
{
public:
	SvgExporterPrivate( const Cfg::Project & cfg, SvgExporter * parent )
		:	ExporterPrivate( cfg, parent )
	{
	}

	//! Create images.
	void createImages( const QString & dir );
}; // class SvgExporterPrivate

void
SvgExporterPrivate::createImages( const QString & dir )
{
	int i = 1;

	for( const Cfg::Page & form : m_cfg.page() )
	{
		const QString fileName = dir + QStringLiteral( "/" ) +
			QString::number( i ) + QStringLiteral( ".svg" );

		QFile file( fileName );

		bool canModify = true;

		if( !file.open( QIODevice::WriteOnly ) )
			canModify = false;

		file.close();

		if( canModify )
		{
			QSvgGenerator svg;
			svg.setFileName( fileName );
			svg.setResolution( c_resolution );

			drawForm( svg, form, c_resolution );

			++i;
		}
		else
			throw SvgExporterException(
				QObject::tr( "Unable to export SVG into %1.\n"
								"File is not writable." )
					.arg( fileName ) );
	}
}


//
// SvgExporter
//

SvgExporter::SvgExporter( const Cfg::Project & project )
	:	Exporter( std::make_unique< SvgExporterPrivate >( project, this ) )
{
}

SvgExporter::~SvgExporter() = default;

void
SvgExporter::exportToDoc( const QString & fileName )
{
	SvgExporterPrivate * d = d_ptr();

	d->createImages( fileName );
}


//
// SvgExporterException
//

SvgExporterException::SvgExporterException( const QString & w )
	:	m_what( w )
{
}

const QString &
SvgExporterException::what() const noexcept
{
	return m_what;
}

} /* namespace Core */

} /* namespace Prototyper */
