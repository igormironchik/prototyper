
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
#include "windows_cfg.hpp"
#include "session_cfg.hpp"
#include "project_widget.hpp"
#include "desc_window.hpp"

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
static const int c_stateVersion = 2;

//
// TopGuiPrivate
//

class TopGuiPrivate {
public:
	TopGuiPrivate( TopGui * parent )
		:	q( parent )
		,	m_projectWindow( 0 )
	{
	}

	~TopGuiPrivate()
	{
		delete m_projectWindow;
	}

	//! Init.
	void init();
	//! Place windows by default.
	void placeByDefault();

	//! Parent.
	TopGui * q;
	//! Project window.
	ProjectWindow * m_projectWindow;
	//! App's windows cfg.
	QString m_appCfgFileName;
	//! App's session cfg.
	QString m_appSessionCfgFileName;
}; // class TopGuiPrivate

void
TopGuiPrivate::init()
{
	m_projectWindow = new ProjectWindow;

	m_projectWindow->postConstruction();

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

			m_projectWindow->move( cfg.projectWindow().x(),
				cfg.projectWindow().y() );
			m_projectWindow->resize( cfg.projectWindow().width(),
				cfg.projectWindow().height() );

			if( cfg.projectWindow().isMaximized() )
				m_projectWindow->showMaximized();

			m_projectWindow->descWindow()->move( cfg.descWindow().x(),
				cfg.descWindow().y() );
			m_projectWindow->descWindow()->resize( cfg.descWindow().width(),
				cfg.descWindow().height() );

			if( cfg.descWindow().isMaximized() )
			{
				m_projectWindow->descWindow()->showMaximized();

				m_projectWindow->descWindow()->hide();
			}

			if( !cfg.state().isEmpty() )
			{
				m_projectWindow->restoreState(
					QByteArray::fromBase64( cfg.state().toLatin1() ),
					c_stateVersion );
			}
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
	m_projectWindow->showMaximized();
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

		Cfg::Window proj;
		proj.setX( d->m_projectWindow->x() );
		proj.setY( d->m_projectWindow->y() );
		proj.setWidth( d->m_projectWindow->width() );
		proj.setHeight( d->m_projectWindow->height() );
		proj.setIsShown( !d->m_projectWindow->isMinimized() );
		proj.setIsMaximized( d->m_projectWindow->isMaximized() );

		cfg.setProjectWindow( proj );

		Cfg::Window desc;
		desc.setX( d->m_projectWindow->descWindow()->x() );
		desc.setY( d->m_projectWindow->descWindow()->y() );
		desc.setWidth( d->m_projectWindow->descWindow()->width() );
		desc.setHeight( d->m_projectWindow->descWindow()->height() );
		desc.setIsShown( d->m_projectWindow->descWindow()->isVisible() );
		desc.setIsMaximized( d->m_projectWindow->descWindow()->isMaximized() );

		cfg.setDescWindow( desc );

		cfg.setState(
			QString( d->m_projectWindow->saveState(
				c_stateVersion ).toBase64() ) );

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
