
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
#include "top_gui.hpp"
#include "project_window.hpp"
#include "props_window.hpp"
#include "tools_window.hpp"
#include "windows_cfg.hpp"
#include "session_cfg.hpp"
#include "project_widget.hpp"

// Qt include.
#include <QApplication>
#include <QDesktopWidget>
#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>


namespace Prototyper {

namespace Core {

static const QString c_appCfgFileName = QLatin1String( "/Prototyper.cfg" );
static const QString c_appSessionCfgFileName =
	QLatin1String( "/Session.cfg" );

//
// TopGuiPrivate
//

class TopGuiPrivate {
public:
	TopGuiPrivate( TopGui * parent )
		:	q( parent )
		,	m_projectWindow( 0 )
		,	m_propsWindow( 0 )
		,	m_toolsWindow( 0 )
	{
	}

	~TopGuiPrivate()
	{
		delete m_projectWindow;
		delete m_propsWindow;
		delete m_toolsWindow;
	}

	//! Init.
	void init();
	//! Place windows by default.
	void placeByDefault();

	//! Parent.
	TopGui * q;
	//! Project window.
	ProjectWindow * m_projectWindow;
	//! Properties window.
	PropsWindow * m_propsWindow;
	//! Tools window.
	ToolsWindow * m_toolsWindow;
	//! App's windows cfg.
	QString m_appCfgFileName;
	//! App's session cfg.
	QString m_appSessionCfgFileName;
}; // class TopGuiPrivate

void
TopGuiPrivate::init()
{
	m_projectWindow = new ProjectWindow;
	m_propsWindow = new PropsWindow;
	m_toolsWindow = new ToolsWindow;

	static const QString cfgFolder =
		QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation );

	m_appCfgFileName = cfgFolder + c_appCfgFileName;
	m_appSessionCfgFileName = cfgFolder + c_appSessionCfgFileName;

	if( !QFileInfo::exists( m_appCfgFileName ) )
		placeByDefault();
	else
	{
		try {
			Cfg::TagWindowsCfg tag;

			QtConfFile::readQtConfFile( tag, m_appCfgFileName,
				QTextCodec::codecForName( "UTF-8" ) );

			Cfg::WindowsCfg cfg = tag.getCfg();

			m_toolsWindow->move( cfg.toolsWindow().x(), cfg.toolsWindow().y() );
			m_toolsWindow->resize( cfg.toolsWindow().width(),
				cfg.toolsWindow().height() );

			if( !cfg.toolsWindow().isShown() )
				m_projectWindow->hideToolsWindow();
			else
				m_toolsWindow->show();

			m_projectWindow->move( cfg.projectWindow().x(),
				cfg.projectWindow().y() );
			m_projectWindow->resize( cfg.projectWindow().width(),
				cfg.projectWindow().height() );

			m_propsWindow->move( cfg.propsWindow().x(),
				cfg.propsWindow().y() );
			m_propsWindow->resize( cfg.propsWindow().width(),
				cfg.propsWindow().height() );

			if( !cfg.propsWindow().isShown() )
				m_projectWindow->hidePropsWindow();
			else
				m_propsWindow->show();
		}
		catch( const QtConfFile::Exception & )
		{
			placeByDefault();
		}
	}

	QString projectFileName;

	try {
		Cfg::TagSession tag;

		QtConfFile::readQtConfFile( tag, m_appSessionCfgFileName,
			QTextCodec::codecForName( "UTF-8" ) );

		projectFileName = tag.getCfg().project();
	}
	catch( const QtConfFile::Exception & )
	{
	}

	if( !projectFileName.isEmpty() &&
		QFileInfo::exists( projectFileName ) )
			m_projectWindow->readProject( projectFileName );
}

void
TopGuiPrivate::placeByDefault()
{
	const QRect r = QApplication::desktop()->availableGeometry();

	const int fw = m_toolsWindow->geometry().x() - m_toolsWindow->x();
	int hh = m_toolsWindow->geometry().y() - m_toolsWindow->y();
	const int offset = 10;
	const int w25 = r.width() / 4;

	if( !hh ) hh = offset * 4;

	const int tw = w25 - offset * 2 - fw * 2;
	const int h = r.height() - offset * 2 - hh - fw;

	m_toolsWindow->move( r.x() + offset, r.y() + offset );
	m_toolsWindow->resize( tw, h );
	m_toolsWindow->show();

	m_projectWindow->move( r.x() + w25 + offset, r.y() + offset );
	m_projectWindow->resize( tw + w25, h );

	m_propsWindow->move( r.x() + w25 * 3 + offset, r.y() + offset );
	m_propsWindow->resize( tw, h );
	m_propsWindow->show();
}


//
// TopGui
//

TopGui::TopGui()
	:	d( new TopGuiPrivate( this ) )
{
}

TopGui::~TopGui()
{
}

static TopGui * topGuiInstance = 0;

void
TopGui::cleanup()
{
	if( topGuiInstance )
	{
		delete topGuiInstance;

		topGuiInstance = 0;
	}
}

TopGui *
TopGui::instance()
{
	if( !topGuiInstance )
	{
		topGuiInstance = new TopGui;

		topGuiInstance->d->init();

		qAddPostRoutine( &TopGui::cleanup );
	}

	return topGuiInstance;
}

ProjectWindow *
TopGui::projectWindow()
{
	return d->m_projectWindow;
}

ToolsWindow *
TopGui::toolsWindow()
{
	return d->m_toolsWindow;
}

PropsWindow *
TopGui::propsWindow()
{
	return d->m_propsWindow;
}

void
TopGui::saveCfg( QWidget * parent )
{
	QFileInfo info( d->m_appCfgFileName );

	QDir dir( info.path() );

	if( !dir.exists() )
	{
		if( !dir.mkpath( info.path() ) )
		{
			QMessageBox::warning( parent,
				tr( "Unable to create folder..." ),
				tr( "Unable to create folder for the configuration files. "
					"Path: \"%1\"." ).arg( info.path() ) );

			return;
		}
	}

	try {
		Cfg::WindowsCfg cfg;

		Cfg::Window tools;
		tools.setX( d->m_toolsWindow->x() );
		tools.setY( d->m_toolsWindow->y() );
		tools.setWidth( d->m_toolsWindow->width() );
		tools.setHeight( d->m_toolsWindow->height() );
		tools.setIsShown( d->m_toolsWindow->isVisible() );

		cfg.setToolsWindow( tools );

		Cfg::Window props;
		props.setX( d->m_propsWindow->x() );
		props.setY( d->m_propsWindow->y() );
		props.setWidth( d->m_propsWindow->width() );
		props.setHeight( d->m_propsWindow->height() );
		props.setIsShown( d->m_propsWindow->isVisible() );

		cfg.setPropsWindow( props );

		Cfg::Window proj;
		proj.setX( d->m_projectWindow->x() );
		proj.setY( d->m_projectWindow->y() );
		proj.setWidth( d->m_projectWindow->width() );
		proj.setHeight( d->m_projectWindow->height() );
		proj.setIsShown( true );

		cfg.setProjectWindow( proj );

		Cfg::TagWindowsCfg tag( cfg );

		QtConfFile::writeQtConfFile( tag, d->m_appCfgFileName,
			QTextCodec::codecForName( "UTF-8" ) );
	}
	catch( const QtConfFile::Exception & x )
	{
		if( parent )
			QMessageBox::warning( parent,
				tr( "Unable to Save Configuration..." ),
				tr( "Unable to save configuration.\n"
					"%1" ).arg( x.whatAsQString() ) );
	}

	try {
		Cfg::Session s;
		s.setProject( d->m_projectWindow->projectFileName() );

		Cfg::TagSession tag( s );

		QtConfFile::writeQtConfFile( tag, d->m_appSessionCfgFileName,
			QTextCodec::codecForName( "UTF-8" ) );
	}
	catch( const QtConfFile::Exception & x )
	{
		if( parent )
			QMessageBox::warning( parent,
				tr( "Unable to Save Session..." ),
				tr( "Unable to save session.\n"
					"%1" ).arg( x.whatAsQString() ) );
	}
}

} /* namespace Core */

} /* namespace Prototyper */
