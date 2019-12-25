
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

#ifndef PROTOTYPER__CORE__EXPORTER_HPP__INCLUDED
#define PROTOTYPER__CORE__EXPORTER_HPP__INCLUDED

// Qt include.
#include <QScopedPointer>

// Prototyper include.
#include "project_cfg.hpp"

QT_BEGIN_NAMESPACE
class QSvgGenerator;
QT_END_NAMESPACE


namespace Prototyper {

namespace Core {

class Exporter;


//
// ExporterPrivate
//

class ExporterPrivate {
public:
	ExporterPrivate( const Cfg::Project & cfg, Exporter * parent );
	virtual ~ExporterPrivate();

	//! Init.
	virtual void init();
	//! Draw form.
	void drawForm( QSvgGenerator & svg, const Cfg::Page & form );

	//! Parent.
	Exporter * q;
	//! Cfg.
	Cfg::Project m_cfg;
}; // class ExporterPrivate


//
// Exporter
//

//! Base class for exporters to the doc.
class Exporter {
public:
	explicit Exporter( const Cfg::Project & project );
	virtual ~Exporter();

	//! Export documentation.
	virtual void exportToDoc( const QString & fileName ) = 0;

protected:
	explicit Exporter( QScopedPointer< ExporterPrivate > && dd );

protected:
	QScopedPointer< ExporterPrivate > d;

private:
	Q_DISABLE_COPY( Exporter )
}; // class Exporter

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__EXPORTER_HPP__INCLUDED
