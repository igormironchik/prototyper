
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
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
class HtmlExporter final
	:	public Exporter
{
public:
	explicit HtmlExporter( const Cfg::Project & project );
	~HtmlExporter() override;

	//! Export documentation.
	void exportToDoc( const QString & fileName ) override;

private:
	inline const HtmlExporterPrivate * d_ptr() const
		{ return reinterpret_cast< const HtmlExporterPrivate* > ( d.get() ); }
	inline HtmlExporterPrivate * d_ptr()
		{ return reinterpret_cast< HtmlExporterPrivate* > ( d.get() ); }

private:
	Q_DISABLE_COPY( HtmlExporter )
}; // class HtmlExporter

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__HTML_EXPORTER_HPP__INCLUDED
