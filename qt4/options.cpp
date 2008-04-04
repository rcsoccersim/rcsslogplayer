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

const double Options::PITCH_LENGTH = 105.0;
const double Options::PITCH_WIDTH = 68.0;
const double Options::PITCH_HALF_LENGTH = Options::PITCH_LENGTH * 0.5;
const double Options::PITCH_HALF_WIDTH = Options::PITCH_WIDTH * 0.5;
const double Options::PITCH_MARGIN = 5.0;
const double Options::CENTER_CIRCLE_R = 9.15;
const double Options::PENALTY_AREA_LENGTH = 16.5;
const double Options::PENALTY_AREA_WIDTH = 40.32;
const double Options::PENALTY_CIRCLE_R = 9.15;
const double Options::PENALTY_SPOT_DIST = 11.0;
const double Options::GOAL_WIDTH = 14.02;
const double Options::GOAL_HALF_WIDTH = Options::GOAL_WIDTH * 0.5;
const double Options::GOAL_AREA_LENGTH = 5.5;
const double Options::GOAL_AREA_WIDTH = 18.32;
const double Options::GOAL_DEPTH = 2.44;
const double Options::CORNER_ARC_R = 1.0;
const double Options::GOAL_POST_RADIUS = 0.06;

const double Options::MIN_FIELD_SCALE = 1.0;
const double Options::MAX_FIELD_SCALE = 400.0;
const double Options::ZOOM_RATIO = 1.5;
const int Options::DEFAULT_TIMER_INTERVAL = 100;

/*-------------------------------------------------------------------*/
/*!
  singleton interface
*/
Options &
Options::instance()
{
    static Options s_instance;
    return s_instance;
}

/*-------------------------------------------------------------------*/
/*!

 */
Options::Options()
    : M_connect( false )
    , M_host( "localhost" )
    , M_port( 6000 )
    , M_client_version( 3 )
    , M_time_shift_replay( true )
    , M_game_log_file( "" )
    , M_auto_loop_mode( true )
    , M_timer_interval( Options::DEFAULT_TIMER_INTERVAL )
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
    , M_show_keepaway_area( false )
    , M_show_team_logo( true )
    , M_show_ball( true )
    , M_show_player( true )
    , M_show_player_number( true )
    , M_show_player_type( false )
    , M_show_view_area( true )
    , M_show_control_area( true )
    , M_show_stamina( true )
    , M_show_pointto( true )
    , M_enlarge( true )
    , M_ball_size( 0.35 )
    , M_player_size( 0.0 )
    , M_grid_step( 5.0 )
    , M_show_grid( false )
    , M_show_grid_coord( false )
    , M_show_flag( false )
    , M_show_offside_line( false )
    , M_field_scale( 1.0 )
    , M_zoomed( false )
    , M_field_center( 0, 0 )
    , M_focus_type( FOCUS_POINT )
    , M_focus_point( 0.0, 0.0 )
    , M_selected_number( 0 )
    , M_player_select_type( SELECT_UNSELECT )
    , M_ball_trace_start( 0 )
    , M_ball_trace_end( 0 )
    , M_player_trace_start( 0 )
    , M_player_trace_end( 0 )
    , M_line_trace( true )
    , M_ball_vel_cycle( 0 )
{

}

/*-------------------------------------------------------------------*/
/*!

 */
Options::~Options()
{

}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Options::parseCmdLine( int argc,
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
        // logplayer options
        ( "auto-loop-mode",
          po::value< bool >( &M_auto_loop_mode )->default_value( true, "on" ),
          "enable automatic replay loop mode." )
        ( "time-shift-replay",
          po::value< bool >( &M_time_shift_replay )->default_value( true, "on" ),
          "enable time shift replay mode." )
        ( "timer-interval",
          po::value< int >( &M_timer_interval )->default_value( DEFAULT_TIMER_INTERVAL ),
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
          po::bool_switch( &M_full_screen ),
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
          po::value< bool >( &M_anti_aliasing )->default_value( true, "on" ),
          "show anti-aliased objects." )
        ( "show-score-board",
          po::value< bool >( &M_show_score_board )->default_value( true, "on" ),
          "show score board." )
        ( "show-keepaway-area",
          po::value< bool >( &M_show_keepaway_area )->default_value( false, "off" ),
          "show keepaway area." )
        ( "show-team-logo",
          po::value< bool >( &M_show_team_logo )->default_value( true, "on" ),
          "show team logo." )
        ( "show-ball",
          po::value< bool >( &M_show_ball )->default_value( true, "on" ),
          "show ball." )
        ( "show-player",
          po::value< bool >( &M_show_player )->default_value( true, "on" ),
          "show players." )
        ( "show-player-number",
          po::value< bool >( &M_show_player_number )->default_value( true, "on" ),
          "show player\'s uniform number." )
        ( "show-player-type",
          po::value< bool >( &M_show_player_type )->default_value( false, "off" ),
          "show player\'s heterogeneous type ID." )
        ( "show-view-area",
          po::value< bool >( &M_show_view_area )->default_value( false, "off" ),
          "show player\'s view area." )
        ( "show-control-area",
          po::value< bool >( &M_show_control_area )->default_value( false, "off" ),
          "show player\'s control area." )
        ( "show-stamina",
          po::value< bool >( &M_show_stamina )->default_value( false, "off" ),
          "show player\'s stamina." )
        ( "show-pointto",
          po::value< bool >( &M_show_pointto )->default_value( false, "off" ),
          "show player\'s pointing to point." )
        ( "enlarge",
          po::value< bool >( &M_enlarge )->default_value( true, "on" ),
          "show enlarged objects." )
        ( "ball-size",
          po::value< double >( &M_ball_size )->default_value( 0.35, "0.35" ),
          "set a ball radius in enlarge mode." )
        ( "player-size",
          po::value< double >( &M_player_size )->default_value( 0.0, "0.0" ),
          "set a fixed player radius in enlarge mode." )
        ( "show-grid",
          po::value< bool >( &M_show_grid )->default_value( false, "off" ),
          "show grid lines." )
        ( "show-grid-coord",
          po::value< bool >( &M_show_grid_coord )->default_value( false, "off" ),
          "show grid line coordinate value." )
        ( "grid-step",
          po::value< double >( &M_grid_step )->default_value( 5.0, "5.0" ),
          "set a grid step size." )
        ( "show-flag",
          po::value< bool >( &M_show_flag )->default_value( false, "off" ),
          "show landmark flags." )
        ( "show-offside-line",
          po::value< bool >( &M_show_offside_line )->default_value( false, "off" ),
          "show offside lines." )
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

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::setFieldScale( const double & value )
{
    if ( std::fabs( M_field_scale - value ) > 0.01 )
    {
        M_field_scale = std::max( MIN_FIELD_SCALE, std::min( value, MAX_FIELD_SCALE ) );
        M_zoomed = true;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::zoomIn()
{
    M_zoomed = true;
    M_field_scale *= ZOOM_RATIO;
    if ( M_field_scale > MAX_FIELD_SCALE )
    {
        M_field_scale = MAX_FIELD_SCALE;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::zoomOut()
{
    M_zoomed = true;
    M_field_scale /= ZOOM_RATIO;
    if ( M_field_scale < MIN_FIELD_SCALE )
    {
        M_field_scale = MIN_FIELD_SCALE;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::unzoom()
{
    if ( M_zoomed
         || focusType() != Options::FOCUS_POINT
         || focusPoint().x() != 0.0
         || focusPoint().y() != 0.0 )
    {
        M_focus_type = Options::FOCUS_POINT;
        M_focus_point.setX( 0.0 );
        M_focus_point.setY( 0.0 );
        M_zoomed = false;

        int temp_width = M_canvas_width;
        M_canvas_width += 1;
        updateFieldSize( temp_width, M_canvas_height );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::updateFieldSize( const int canvas_width,
                          const int canvas_height )
{
    if ( M_canvas_width != canvas_width
         || M_canvas_height != canvas_height )
    {
        M_canvas_width = canvas_width;
        M_canvas_height = canvas_height;

        // adjust field scale to window size.
        if ( ! zoomed() )
        {
            double total_pitch_l = ( PITCH_LENGTH
                                     + PITCH_MARGIN * 2.0
                                     + 1.0 );
            double total_pitch_w = ( PITCH_WIDTH
                                     + PITCH_MARGIN * 2.0
                                     + 1.0 );

            M_field_scale = static_cast< double >( canvas_width ) / total_pitch_l;

            // automatically adjust a field scale
            if ( total_pitch_w * fieldScale() > canvas_height )
            {
                M_field_scale = static_cast< double >( canvas_height ) / total_pitch_w;
            }

            // check the scale threshold
            if ( fieldScale() < MIN_FIELD_SCALE )
            {
                M_field_scale = MIN_FIELD_SCALE;
            }

            M_field_scale = rint( M_field_scale * 100.0 ) / 100.0;
        }
    }

    M_field_center.setX( canvas_width/2 - scale( focusPoint().x() ) );
    M_field_center.setY( canvas_height/2 - scale( focusPoint().y() ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::setFocusPoint( const int screen_x,
                        const int screen_y )
{
    M_focus_type = Options::FOCUS_POINT;

    M_focus_point.setX( fieldX( screen_x ) );
    M_focus_point.setY( fieldY( screen_y ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::unselectPlayer()
{
    M_player_select_type = Options::SELECT_UNSELECT;
    setSelectedNumber( rcss::rcg::NEUTRAL, 0 );
    M_focus_type = Options::FOCUS_POINT;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Options::setPlayerSelectType( const Options::PlayerSelectType type )
{
    if ( type != Options::SELECT_FIX
         && type == M_player_select_type )
    {
        unselectPlayer();
    }
    else
    {
        M_player_select_type = type;
    }
}
