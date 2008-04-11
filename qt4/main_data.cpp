// -*-c++-*-

/*!
	\file main_data.cpp
	\brief main data class Source File.
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.	If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui>

#include "main_data.h"

#include "options.h"

#ifdef HAVE_LIBZ
#include <rcsslogplayer/gzfstream.h>
#endif
#include <rcsslogplayer/util.h>
#include <rcsslogplayer/parser.h>

#include <fstream>
#include <iostream>
#include <cmath>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

namespace {

inline
float
quantize( const float & val,
          const float & prec )
{
    return rintf( val / prec ) * prec;
}

}

/*-------------------------------------------------------------------*/
/*!

*/
MainData::MainData()
    : M_out( static_cast< std::ostream * >( 0 ) )
    , M_record_mode( false )
    , M_record_playmode( rcss::rcg::PM_Null )
{

}

/*-------------------------------------------------------------------*/
/*!

*/
MainData::~MainData()
{
    closeOutputFile();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MainData::clear()
{
    M_index = 0;
    M_disp_holder.clear();
}

/*-------------------------------------------------------------------*/
/*!
  \todo multi-threaded
*/
bool
MainData::openRCG( const QString & file_path,
                   QWidget * parent )
{
#ifdef HAVE_LIBZ
    rcss::gzifstream fin( file_path.toAscii() );
#else
    std::ifstream fin( file_path.toAscii() );
#endif

    if ( ! fin )
    {
        std::cerr << "failed to open the rcg file. [" << file_path.toStdString() << "]"
                  << std::endl;
        return false;
    }

    clear();


    // show progress dialog
    QProgressDialog progress_dialog( parent );
    progress_dialog.setWindowTitle( QObject::tr( "parsing rcg file..." ) );
    progress_dialog.setRange( 0, 6000 );
    progress_dialog.setValue( 0 );
    progress_dialog.setLabelText( QObject::tr( "Time: 0" ) );
    progress_dialog.setCancelButton( 0 ); // no cancel button
    progress_dialog.setMinimumDuration( 0 ); // no duration

    QTime timer;
    timer.start();

    rcss::rcg::Parser parser( M_disp_holder );
    int count = 0;
    while (  parser.parse( fin ) )
    {
        ++count;

        if ( count % 32 == 1 )
        {
            if ( ! M_disp_holder.dispInfoCont().empty() )
            {
                int time = M_disp_holder.dispInfoCont().back()->show_.time_;
                if ( time > progress_dialog.maximum() )
                {
                    progress_dialog.setMaximum( progress_dialog.maximum() + 6000 );
                }
                progress_dialog.setValue( time );
                progress_dialog.setLabelText( QString( "Time: %1" ).arg( time ) );
            }
        }

        if ( count % 512 == 1 )
        {
            qApp->processEvents();
            std::fprintf( stdout, "parsing... %d\r", count );
            std::fflush( stdout );
        }
    }

    std::cerr << "parsing elapsed " << timer.elapsed() << " [ms]" << std::endl;

    if ( ! fin.eof() )
    {
        std::cerr << "failed to parse the rcg file [" << file_path.toStdString() << "]."
                  << std::endl;
        fin.close();
        return false;
    }

    fin.close();

    std::cerr << "opened rcg file [" << file_path.toStdString()
              << "]. data size = "
              << M_disp_holder.dispInfoCont().size()
              << std::endl;

    Options::instance().setGameLogFile( file_path.toStdString() );
    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::openOutputFile( const QString & file_path )
{
    closeOutputFile();

    if ( M_disp_holder.dispInfoCont().empty() )
    {
        return false;
    }

    M_out = new std::ofstream( file_path.toAscii(),
                               std::ios_base::out
                               | std::ios_base::trunc
                               | std::ios_base::binary );

    if ( ! *M_out )
    {
        delete M_out;
        M_out = static_cast< std::ostream * >( 0 );
    }

    std::cerr << "opened output file [" << file_path.toStdString() << ']'
              << std::endl;

    // write header

    if ( dispHolder().logVersion() == rcss::rcg::REC_VERSION_4 )
    {
        *M_out << "ULG4\n";
        {
            serverParam().print( *M_out ) << '\n';
        }
        {
            playerParam().print( *M_out ) << '\n';
        }

        for ( std::map< int, rcss::rcg::PlayerTypeT >::const_iterator it = playerTypes().begin();
              it != playerTypes().end();
              ++it )
        {
            it->second.print( *M_out ) << '\n';
        }
    }
    else if ( dispHolder().logVersion() == rcss::rcg::REC_VERSION_3 )
    {
        char header[5];
        header[0] = 'U';
        header[1] = 'L';
        header[2] = 'G';
        header[3] = static_cast< char >( rcss::rcg::REC_VERSION_3 );
        M_out->write( header, 4 );

        rcss::rcg::dispinfo_t2 disp2;

        disp2.mode = htons( rcss::rcg::PARAM_MODE );
        rcss::rcg::convert( serverParam(), disp2.body.sparams );
        M_out->write( reinterpret_cast< const char * >( &disp2.mode ),
                      sizeof( rcss::rcg::Int16 ) );
        M_out->write( reinterpret_cast< const char * >( &disp2.body.sparams ),
                      sizeof( rcss::rcg::server_params_t ) );

        disp2.mode = htons( rcss::rcg::PPARAM_MODE );
        rcss::rcg::convert( playerParam(), disp2.body.pparams );
        M_out->write( reinterpret_cast< const char * >( &disp2.mode ),
                      sizeof( rcss::rcg::Int16 ) );
        M_out->write( reinterpret_cast< const char * >( &disp2.body.pparams ),
                      sizeof( rcss::rcg::player_params_t ) );

        disp2.mode = htons( rcss::rcg::PT_MODE );
        for ( std::map< int, rcss::rcg::PlayerTypeT >::const_iterator it = playerTypes().begin();
              it != playerTypes().end();
              ++it )
        {
            rcss::rcg::convert( it->second, disp2.body.ptinfo );
            M_out->write( reinterpret_cast< const char * >( &disp2.mode ),
                          sizeof( rcss::rcg::Int16 ) );
            M_out->write( reinterpret_cast< const char * >( &disp2.body.ptinfo ),
                          sizeof( rcss::rcg::player_type_t ) );
        }

    }
    else if ( dispHolder().logVersion() == rcss::rcg::REC_VERSION_2 )
    {
        char header[5];
        header[0] = 'U';
        header[1] = 'L';
        header[2] = 'G';
        header[3] = static_cast< char >( rcss::rcg::REC_VERSION_2 );
        M_out->write( header, 4 );
    }

    M_out->flush();

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MainData::setEnableRecord( bool checked )
{
    M_record_mode = checked;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MainData::outputCurrentData()
{
    if ( ! M_record_mode
         || ! M_out
         || ! *M_out )
    {
        return;
    }

    DispConstPtr disp = getDispInfo( index() );

    if ( ! disp )
    {
        std::cerr << "MainData::outputCurrentData. no data! " << std::endl;
        return;
    }

    if ( dispHolder().logVersion() == rcss::rcg::REC_VERSION_4 )
    {
        serializeShow( *M_out, *disp );
    }
    else if ( dispHolder().logVersion() == rcss::rcg::REC_VERSION_3 )
    {
        rcss::rcg::Int16 mode;

        if ( M_record_playmode != disp->pmode_ )
        {
            M_record_playmode = disp->pmode_;
            char pm = static_cast< char >( M_record_playmode );
            mode = htons( rcss::rcg::PM_MODE );
            M_out->write( reinterpret_cast< const char * >( &mode ),
                          sizeof( rcss::rcg::Int16 ) );
            M_out->write( reinterpret_cast< const char * >( &pm ),
                          sizeof( char ) );
        }
        if ( ! M_record_team[0].equals( disp->team_[0] )
             || ! M_record_team[1].equals( disp->team_[1] ) )
        {
            M_record_team[0] = disp->team_[0];
            M_record_team[1] = disp->team_[1];

            rcss::rcg::team_t team[2];
            rcss::rcg::convert( M_record_team[0], team[0] );
            rcss::rcg::convert( M_record_team[1], team[1] );
            mode = htons( rcss::rcg::TEAM_MODE );
            M_out->write( reinterpret_cast< const char * >( &mode ),
                          sizeof( rcss::rcg::Int16 ) );
            M_out->write( reinterpret_cast< const char * >( team ),
                          sizeof( rcss::rcg::team_t ) * 2 );
        }

        rcss::rcg::short_showinfo_t2 show;
        rcss::rcg::convert( disp->show_,
                            show );
        mode = htons( rcss::rcg::SHOW_MODE );
        M_out->write( reinterpret_cast< const char * >( &mode ),
                     sizeof( rcss::rcg::Int16 ) );
        M_out->write( reinterpret_cast< const char * >( &show ),
                     sizeof( rcss::rcg::short_showinfo_t2 ) );
    }
    else if ( dispHolder().logVersion() == rcss::rcg::REC_VERSION_2 )
    {
        rcss::rcg::Int16 mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::showinfo_t show;
        rcss::rcg::convert( static_cast< char >( disp->pmode_ ),
                            disp->team_[0],
                            disp->team_[1],
                            disp->show_,
                            show );
        M_out->write( reinterpret_cast< const char * >( &mode ),
                     sizeof( rcss::rcg::Int16 ) );
        M_out->write( reinterpret_cast< const char * >( &show ),
                     sizeof( rcss::rcg::showinfo_t ) );
    }
    else if ( dispHolder().logVersion() == rcss::rcg::REC_OLD_VERSION )
    {
        rcss::rcg::dispinfo_t new_disp;
        new_disp.mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::convert( static_cast< char >( disp->pmode_ ),
                            disp->team_[0],
                            disp->team_[1],
                            disp->show_,
                            new_disp.body.show );
        M_out->write( reinterpret_cast< const char * >( &new_disp ),
                      sizeof( rcss::rcg::dispinfo_t ) );
    }

    M_out->flush();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MainData::closeOutputFile()
{
    if ( M_out )
    {
        M_out->flush();
        delete M_out;
        M_out = static_cast< std::ostream * >( 0 );
    }
    M_record_mode = false;
    M_record_playmode = rcss::rcg::PM_Null;
    M_record_team[0].clear();
    M_record_team[1].clear();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MainData::serializeShow( std::ostream & os,
                         const rcss::rcg::DispInfoT & disp )

{
    static const std::string s_playmode_strings[] = PLAYMODE_STRINGS;

    const float PREC = 0.0001f;
    const float DPREC = 0.001f;

    if ( M_record_playmode != disp.pmode_ )
    {
        M_record_playmode = disp.pmode_;
        os << "(playmode " << disp.show_.time_
           << ' ' << s_playmode_strings[M_record_playmode]
           << ")\n";
    }

    if ( ! M_record_team[0].equals( disp.team_[0] )
         || ! M_record_team[1].equals( disp.team_[1] ) )
    {
        M_record_team[0] = disp.team_[0];
        M_record_team[1] = disp.team_[1];

        os << "(team " << disp.show_.time_
             << ' ' << ( disp.team_[0].name_.empty() ? "null" : disp.team_[0].name_.c_str() )
             << ' ' << ( disp.team_[1].name_.empty() ? "null" : disp.team_[1].name_.c_str() )
             << ' ' << disp.team_[0].score_
             << ' ' << disp.team_[1].score_;
        if ( disp.team_[0].penaltyTrial() > 0
             || disp.team_[1].penaltyTrial() > 0 )
        {
            os << ' ' << disp.team_[0].pen_score_
               << ' ' << disp.team_[0].pen_miss_
               << ' ' << disp.team_[0].pen_score_
               << ' ' << disp.team_[0].pen_miss_;
        }
        os << ")\n";
    }

    os << "(show " << disp.show_.time_;

    os << " ((b)"
       << ' ' << quantize( disp.show_.ball_.x_, PREC )
       << ' ' << quantize( disp.show_.ball_.y_, PREC );
    if ( disp.show_.ball_.hasVelocity() )
    {
        os << ' ' << quantize( disp.show_.ball_.vx_, PREC )
           << ' ' << quantize( disp.show_.ball_.vy_, PREC );
    }
    else
    {
        os << " 0 0";
    }
    os << ')';

    for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
    {
        const rcss::rcg::PlayerT & p = disp.show_.player_[i];

        os << " ("
           << '(' << p.side_ << ' ' << p.unum_ << ')'
           << ' ' << p.type_
           << ' ' << std::hex << std::showbase << p.state_ << std::dec << std::noshowbase;
        os << ' ' << quantize( p.x_, PREC )
           << ' ' << quantize( p.y_, PREC );

        if ( p.hasVelocity() )
        {
            os << ' ' << quantize( p.vx_, PREC )
               << ' ' << quantize( p.vy_, PREC );
        }
        else
        {
            os << " 0 0";
        }

        os << ' ' << quantize( p.body_, DPREC )
           << ' ' << ( p.hasNeck() ? quantize( p.neck_, DPREC ) : 0.0 );
        if ( p.isPointing() )
        {
            os << ' ' << quantize( p.point_x_, PREC )
               << ' ' << quantize( p.point_y_, PREC );
        }
        if ( p.hasView() )
        {
            os << " (v " << p.view_quality_ << ' ' << quantize( p.view_width_, DPREC ) << ')';
        }
        else
        {
            os << " (v h 90)";
        }
        if ( p.hasStamina() )
        {
            os << " (s "
               << quantize( p.stamina_, 0.001f )<< ' '
               << quantize( p.effort_, 0.0001f ) << ' '
               << quantize( p.recovery_, 0.0001f ) << ')';
        }
        else
        {
            os << " (s 4000 1 1)";
        }
        os << " (c "
           << p.kick_count_ << ' '
           << p.dash_count_ << ' '
           << p.turn_count_ << ' '
           << p.catch_count_ << ' '
           << p.move_count_ << ' '
           << p.turn_neck_count_ << ' '
           << p.change_view_count_ << ' '
           << p.say_count_ << ' '
           << p.tackle_count_ << ' '
           << p.pointto_count_ << ' '
           << p.attentionto_count_ << ')';
        os << ')';
    }

    os << ")\n";
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::setIndexFirst()
{
    M_index = 0;

    return ( ! M_disp_holder.dispInfoCont().empty() );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::setIndexLast()
{
    if ( M_disp_holder.dispInfoCont().empty() )
    {
        M_index = 0;
        return false;
    }

    M_index = M_disp_holder.dispInfoCont().size() - 1;
    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::setIndexStepBack()
{
    if ( 0 < M_index )
    {
        --M_index;
        return true;
    }
    else
    {
        if ( Options::instance().autoLoopMode() )
        {
            M_index = dispHolder().dispInfoCont().size() - 1;
            return true;
        }
        else
        {
            return false;
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::setIndexStepForward()
{
    if ( M_index < dispHolder().dispInfoCont().size() - 1 )
    {
        ++M_index;
        return true;
    }
    else
    {
        if ( Options::instance().autoLoopMode() )
        {
            M_index = 0;
            return true;
        }
        else
        {
            return false;
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::setIndex( const int index )
{
    if ( static_cast< int >( M_index ) == index )
    {
        return false;
    }

    if ( index < 0 )
    {
        return false;
    }

    std::size_t idx = static_cast< std::size_t >( index );

    if ( idx >= dispHolder().dispInfoCont().size() )
    {
        return false;
    }

    M_index = idx;

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MainData::setCycle( const int cycle )
{

    std::size_t index = M_disp_holder.getIndexOf( cycle );

    if ( index == M_index )
    {
        return false;
    }

    M_index = index;
    return true;
}
