// -*-c++-*-

/*!
  \file options.cpp
  \brief application configuration class Source File.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "rcsslogplayer"
#endif
#ifndef VERSION
#define VERSION "unknown-version"
#endif

#include "options.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <cmath>
#include <cstdio>


//! default logplayer timer step (ms).
const int AppConfig::DEFAULT_TIMER_INTERVAL = 100;

/*-------------------------------------------------------------------*/
/*!
  singleton interface
*/
AppConfig &
AppConfig::instance()
{
    static AppConfig s_instance;
    return s_instance;
}

/*-------------------------------------------------------------------*/
/*!

*/
AppConfig::AppConfig()
    // monitor client options
    : M_connect( false )
    , M_host( "localhost" )
    , M_port( 6000 )
    , M_client_version( 3 )
    , M_wait_seconds( 5 )
    , M_auto_quit_mode( false )
    , M_time_shift_replay( true )
      // logplayer options
    , M_game_log_file( "" )
    , M_auto_loop_mode( false )
    , M_timer_interval( DEFAULT_TIMER_INTERVAL )
      // window options
    , M_window_x( -1 )
    , M_window_y( -1 )
    , M_window_width( -1 )
    , M_window_height( -1 )
    , M_maximize( false )
    , M_full_screen( false )
    , M_canvas_width( -1 )
    , M_canvas_height( -1 )
    , M_hide_menu_bar( false )
    , M_hide_tool_bar( false )
    , M_hide_status_bar( false )
      // view options
    , M_anti_aliasing( true )
    , M_show_score_board( true )
    , M_show_team_logo( true )
    , M_show_ball( true )
    , M_show_player( true )
    , M_show_player_number( true )
    , M_show_player_type( false )
    , M_show_view_area( true )
    , M_show_control_area( true )
    , M_show_stamina( true )
    , M_show_pointto( true )
    , M_ball_size( 0.35 )
    , M_player_size( 0.0 )
    , M_grid_step( 5.0 )
    , M_show_grid( false )
    , M_show_grid_coord( false )
{

}

/*-------------------------------------------------------------------*/
/*!

*/
AppConfig::~AppConfig()
{

}

/*-------------------------------------------------------------------*/
/*!
  analyze command line options
*/
bool
AppConfig::parseCmdLine( int argc,
                         char ** argv )
{
    namespace po = boost::program_options;

    std::string geometry;
    std::string canvas_size;

    po::options_description visibles( "Allowed options:" );

    visibles.add_options()
        ( "help,h",
          "print this message." )
        ( "version,v",
          "print version information." )
        // monitor options
        ( "connect,c",
          po::bool_switch( &M_connect ),
          "start as a soccer monitor." )
        ( "host",
          po::value< std::string >( &M_host )->default_value( "localhost", "localhost" ),
          "set host name to be connected." )
        ( "port",
          po::value< int >( &M_port )->default_value( 6000, "6000" ),
          "set port number for the monitor client and monitor server." )
        ( "client-version",
          po::value< int >( &M_client_version )->default_value( 3, "3" ),
          "set a monitor client protocol version." )
        ( "wait-seconds",
          po::value< int >( &M_wait_seconds )->default_value( 5, "5", ),
          "set maximal seconds to wait a message from rcssserver." )
        ( "auto-quit-mode",
          po::bool_switch( &M_auto_quit_mode ),
          "enable automatic quit after game end." )
        ( "time-shift-replay",
          po::value< bool >( &M_time_shift_replay )->default_value( true, "true" ),
          "enable time shift replay mode." )
        // logplayer options
        ( "auto-loop-mode", "",
          po::value< bool >( &M_auto_loop_mode )->default_value( false, "false" ),
          "enable automatic replay loop." )
        ( "timer-interval",
          po:value< int >( &M_timer_interval )->default_value( DEFAULT_TIMER_INTERVAL ),
          "set the logplayer timer interval." )
        // window options
        ( "geometry",
          po::value< std::string >( &geometry )->default_value( "" ),
          "specifies the window geometry ([WxH][+X+Y]). e.g. --geometry=1024x768+1+1" )
        ( "canvas-size",
          po::value< std::string >( &canvas_size )->default_value( "" ),
          "specifies the canvas size(WxH). e.g. --canvas-size=1024x768" )
        ( "maximize",
          po::bool_switch( &M_maximize ),
          "start with a maximized window." )
        ( "full-screen",
          po::bool_siwtch( &M_full_screen ),
          "start with a full screen window." )
        ( "hide-menu-bar",
          po::bool_switch( &M_hide_menu_bar ),
          "start without a menu bar (available only by Qt3 version)." )
        ( "hide-tool-bar",
          po::bool_switch( &M_hide_tool_bar ),
          "start without a tool bar." )
        ( "hide-status-bar",
          po::bool_switch( &M_hide_status_bar ),
          "start without a status bar." )
        // view options
        ( "anti-aliasing",
          po::value< bool >( &M_anti_aliasing )->default_value( true, "true" ),
          "show anti-aliased objects." )
        ( "show-score-board",
          po::value< bool >( &M_show_score_board )->default_value( true, "true" ),
          "show score board." )
        ( "show-team-logo",
          po::value< bool >( &M_show_team_logo )->default_value( true, "true" ),
          "show team logo." )
        ( "show-ball",
          po::value< bool >( &M_show_ball )->default_value( true, "true" ),
          "show ball." )
        ( "show-player",
          po::value< bool >( &M_show_player )->default_value( true, "true" ),
          "show players." )
        ( "show-player-number",
          po::value< bool >( &M_show_player_number )->default_value( true, "true" ),
          "show player\'s uniform number." )
        ( "show-player-type",
          po::value< bool >( &M_show_player_type )->default_value( false, "false" ),
          "show player\'s heterogeneous type ID." )
        ( "show-view-area",
          po::value< bool >( &M_show_view_area )->default_value( false, "false" ),
          "show player\'s view area." )
        ( "show-control-area",
          po::value< bool >( &M_show_control_area )->default_value( false, "false" ),
          "show player\'s control area." )
        ( "show-stamina",
          po::value< bool >( &M_show_stamina )->default_value( false, "false" ),
          "show player\'s stamina." )
        ( "show-pointto",
          po::value< bool >( &M_show_pointto )->default_value( false, "false" ),
          "show player\'s pointing to point." )
        ( "ball-size",
          po::value< double >( &M_ball_size )->default_value( 0.35, "0.35" ),
          "set a ball radius in enlarge mode." )
        ( "player-size",
          po::value< double >( &M_player_size )->default_value( 0.0, "0.0" ),
          "set a fixed player radius in enlarge mode." )
        ( "show-grid",
          po::value< bool >( &M_show_grid )->default_value( false, "false" ),
          "show grid lines." )
        ( "show-grid-coord",
          po::value< bool >( &M_show_grid_coord )->default_value( false, "false" ),
          "show grid line coordinate value." )
        ( "grid-step",
          po::value< bool >( &M_grid_step )->default_value( 5.0, "5.0" ),
          "set a grid step size." )
        ;

    po::options_description invisibles( "Invisibles" );
    invisibles.add_options()
        ( "game-log",
          po::value< std::string >( &M_game_log_file )->default_value( "" ),
          "set the path to Game Log file(.rcg) to be opened.")
        ;

    po::positional_options_description pdesc;
    pdesc.add( "game-log", 1 );

    po::options_description all_desc( "All options:" );
    all_desc.add( visibles ).add( invisibles );


    bool help = false;
    bool version = false;
    try
    {
        po::variables_map vm;
        po::command_line_parser parser( argc, argv );
        parser.options( all_desc ).positional( pdesc );
        po::store( parser.run(), vm );
        po::notify( vm );

        help = vm.count( "help" );
        version = vm.count( "version" );
    }
    catch ( std::exception & e )
    {
        std::cerr << e.what() << std::endl;
        help = true;
    }

    if ( version )
    {
        std::cout << PACKAGE_NAME << " Version " << VERSION
                  << std::endl;
        if ( ! help )
        {
            return false;
        }
    }

    if ( help )
    {
        std::cout << "Usage: " << PACKAGE_NAME
                  << " [options ... ] [<GameLogFile>]\n";
        std::cout << visibles << std::endl;
        return false;
    }

    if ( M_timer_interval < 0 )
    {
        std::cerr << "Illegal timer interval " << M_timer_interval
                  << ". set default value." << std::endl;
        M_timer_interval = DEFAULT_TIMER_INTERVAL;
    }

    if ( M_timer_interval < 5 )
    {
        std::cerr << "Too small timer interval " << M_timer_interval
                  << ".  replaced by 5." << std::endl;
        M_timer_interval = 5;
    }

    if ( M_timer_interval > 5000 )
    {
        std::cerr << "Too huge timer interval " << M_timer_interval
                  << ".  replaced by 5000." << std::endl;
        M_timer_interval = 5000;
    }

    if ( ! geometry.empty() )
    {
        int w = -1, h = -1;
        int x = -1, y = -1;

        int num = std::sscanf( geometry.c_str(),
                               " %d x %d %d %d " ,
                               &w, &h, &x, &y );
        //std::cerr << "geometry = " << geometry
        //          << "  param_num=" << num
        //          << " width=" << w << " height=" << h
        //          << " x=" << x << " y=" << y
        //          << std::endl;
        if ( num == 4 || num == 2 )
        {
            if ( w <= 0 || h <= 0 )
            {
                std::cerr << "Illegal window size [" << geometry
                          << "]" << std::endl;
            }
            else
            {
                M_window_width = w;
                M_window_height = h;
                M_window_x = x;
                M_window_y = y;
            }
        }
        else if ( std::sscanf( geometry.c_str(),
                               " %d %d " ,
                               &x, &y ) == 2 )
        {
            //std::cerr << "only pos = "
            //          << " x=" << x
            //          << " y=" << y
            //          << std::endl;
            M_window_x = x;
            M_window_y = y;
        }
        else
        {
            std::cerr << "Illegal geometry format [" << geometry
                      << "]" << std::endl;
        }
    }

    if ( ! canvas_size.empty() )
    {
        int w = -1, h = -1;
        if ( std::sscanf( canvas_size.c_str(),
                          " %d x %d ",
                          &w, &h ) == 2
             && w > 1
             && h > 1 )
        {
            M_canvas_width = w;
            M_canvas_height = h;
        }
        else
        {
            std::cerr << "Illegal canvas size format [" << canvas_size
                      << "]" << std::endl;
        }
    }

    return true;
}
