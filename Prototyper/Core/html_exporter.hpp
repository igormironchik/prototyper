
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

#ifndef PROTOTYPER__CORE__HTML_EXPORTER_HPP__INCLUDED
#define PROTOTYPER__CORE__HTML_EXPORTER_HPP__INCLUDED

// Prototyper include.
#include "exporter.hpp"


namespace Prototyper {

namespace Core {

//
// HtmlExporter
//

class HtmlExporterPrivate;

//! Exporter to HTML.
class HtmlExporter
	:	public Exporter
{
public:
	explicit HtmlExporter( const Cfg::Project & project );
	~HtmlExporter();

	//! Export documentation.
	void exportToDoc( const QString & fileName ) const
		Q_DECL_OVERRIDE;

private:
	inline const HtmlExporterPrivate * d_ptr() const
		{ return reinterpret_cast< const HtmlExporterPrivate* > ( d.data() ); }
	inline HtmlExporterPrivate * d_ptr()
		{ return reinterpret_cast< HtmlExporterPrivate* > ( d.data() ); }

private:
	Q_DISABLE_COPY( HtmlExporter )
}; // class HtmlExporter

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__HTML_EXPORTER_HPP__INCLUDED
