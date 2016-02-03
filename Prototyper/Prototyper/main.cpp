
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

// Qt include.
#include <QApplication>
#include <QIcon>

// Prototyper include.
#include <Prototyper/Core/top_gui.hpp>
#include <Prototyper/Core/project_window.hpp>


using namespace Prototyper::Core;


int main( int argc, char ** argv )
{
	QApplication app( argc, argv );

	QIcon appIcon( ":/img/Prototyper_256x256.png" );
	appIcon.addFile( ":/img/Prototyper_128x128.png" );
	appIcon.addFile( ":/img/Prototyper_64x64.png" );
	appIcon.addFile( ":/img/Prototyper_48x48.png" );
	appIcon.addFile( ":/img/Prototyper_32x32.png" );
	appIcon.addFile( ":/img/Prototyper_22x22.png" );
	appIcon.addFile( ":/img/Prototyper_16x16.png" );
	app.setWindowIcon( appIcon );

	TopGui::instance()->projectWindow()->show();
	
	return app.exec();
}
