
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

#ifndef PROTOTYPER__CORE__SVG_EXPORTER_HPP__INCLUDED
#define PROTOTYPER__CORE__SVG_EXPORTER_HPP__INCLUDED

// Prototyper include.
#include "exporter.hpp"


namespace Prototyper {

namespace Core {

//
// SvgExporter
//

class SvgExporterPrivate;

//! Exporter to SVG.
class SvgExporter final
	:	public Exporter
{
public:
	explicit SvgExporter( const Cfg::Project & project );
	~SvgExporter();

	//! Export documentation.
	void exportToDoc( const QString & fileName ) override;

private:
	inline const SvgExporterPrivate * d_ptr() const
		{ return reinterpret_cast< const SvgExporterPrivate* > ( d.data() ); }
	inline SvgExporterPrivate * d_ptr()
		{ return reinterpret_cast< SvgExporterPrivate* > ( d.data() ); }

private:
	Q_DISABLE_COPY( SvgExporter )
}; // class SvgExporter


//
// SvgExporterException
//

class SvgExporterException final
{
public:
	explicit SvgExporterException( const QString & w );

	const QString & what() const noexcept;

private:
	QString m_what;
}; // class SvgExporterException

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__PDF_EXPORTER_HPP__INCLUDED
