
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

#ifndef PROTOTYPER__CORE__EXPORTER_PRIVATE_HPP__INCLUDED
#define PROTOTYPER__CORE__EXPORTER_PRIVATE_HPP__INCLUDED

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
	virtual ~ExporterPrivate() = default;

	//! Init.
	virtual void init();
	//! Draw form.
	void drawForm( QSvgGenerator & svg, const Cfg::Page & form, qreal dpi );

	//! Parent.
	Exporter * q;
	//! Cfg.
	Cfg::Project m_cfg;
}; // class ExporterPrivate

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__EXPORTER_PRIVATE_HPP__INCLUDED
