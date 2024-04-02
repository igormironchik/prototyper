
/*
	SPDX-FileCopyrightText: 2016-2024 Igor Mironchik <igor.mironchik@gmail.com>
	SPDX-License-Identifier: GPL-3.0-or-later
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
