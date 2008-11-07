// -*-c++-*-

/*!
  \file remote_monitor.cpp
  \brief remote monitor client class Source File.
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

#include <QtNetwork>

#include "remote_monitor.h"

#include <rcsslogplayer/types.h>
#include <rcsslogplayer/util.h>

#include <sstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <cassert>

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
RemoteMonitor::RemoteMonitor( QObject * parent,
                              const int version )

    : QObject( parent )
    , M_alive( false )
    , M_port( 0 )
    , M_socket( new QUdpSocket( this ) )
    , M_version( version )
{
    assert( parent );

    // check protocl versin range
    if ( version < 1 )
    {
        M_version = 1;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
RemoteMonitor::~RemoteMonitor()
{
    disconnect();
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
RemoteMonitor::connect( const QHostAddress & addr,
                        const quint16 port )
{
    //M_socket->connectToHost( addr, port );
    M_socket->bind( addr, port );

    if ( ! isConnected() )
    {
        std::cerr << "RemoteMonitor. failed to connect. host="
                  << addr.toString().toStdString()
                  << " port=" << port
                  << std::endl;
        return false;
    }

    M_alive = true;
    M_addr = addr;
    M_port = port;

    QObject::connect( M_socket, SIGNAL( readyRead() ),
                      this, SLOT( handleReceive() ) );

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
RemoteMonitor::disconnect()
{
    if ( isConnected() )
    {
        M_socket->close();
        //M_socket->disconnectFromHost();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
RemoteMonitor::isConnected() const
{
//     std::cerr << "RemoteMonitor state="
//               << M_socket->state()
//               << " error="
//               << M_socket->error()
//               << " FD=" << M_socket->socketDescriptor()
//               << " valid=" << M_socket->isValid()
//               << std::endl;

    return M_socket
        && M_socket->isValid();
}

/*-------------------------------------------------------------------*/
/*!

 */
int
RemoteMonitor::send( const char * msg,
                     const std::size_t len )
{
    qint64 n = M_socket->writeDatagram( msg, len, M_addr, M_port );
    return static_cast< int >( n );
}

/*-------------------------------------------------------------------*/
/*!

 */
int
RemoteMonitor::send( const rcss::rcg::DispInfoT & disp )
{
    if ( version() >= 3 )
    {
        std::string msg;
        serializeDisp( disp, msg );
        return send( msg.c_str(), msg.length() + 1 );
    }
    else if ( version() == 2 )
    {
        rcss::rcg::dispinfo_t2 new_disp2;
        new_disp2.mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::convert( static_cast< char >( disp.pmode_ ),
                            disp.team_[0],
                            disp.team_[1],
                            disp.show_,
                            new_disp2.body.show );
        return send( reinterpret_cast< const char * >( &new_disp2 ),
                     sizeof( rcss::rcg::dispinfo_t2 ) );
    }
    else if ( version() == 1 )
    {
        rcss::rcg::dispinfo_t new_disp;
        new_disp.mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::convert( static_cast< char >( disp.pmode_ ),
                            disp.team_[0],
                            disp.team_[1],
                            disp.show_,
                            new_disp.body.show );
        return send( reinterpret_cast< const char * >( &new_disp ),
                     sizeof( rcss::rcg::dispinfo_t ) );
    }

    return -1;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
RemoteMonitor::serializeDisp( const rcss::rcg::DispInfoT & disp,
                              std::string & msg )
{
    const float PREC = 0.0001f;
    const float DPREC = 0.001f;

    std::ostringstream ostr;

    ostr << "(show " << disp.show_.time_;

    {
        ostr << " (pm " << disp.pmode_ << ")";
        ostr << " (tm"
             << ' ' << ( disp.team_[0].name_.empty() ? "null" : disp.team_[0].name_.c_str() )
             << ' ' << ( disp.team_[1].name_.empty() ? "null" : disp.team_[1].name_.c_str() )
             << ' ' << disp.team_[0].score_
             << ' ' << disp.team_[1].score_;
        if ( disp.team_[0].penaltyTrial() > 0
             || disp.team_[1].penaltyTrial() > 0 )
        {
            ostr << ' ' << disp.team_[0].pen_score_
                 << ' ' << disp.team_[0].pen_miss_
                 << ' ' << disp.team_[0].pen_score_
                 << ' ' << disp.team_[0].pen_miss_;
        }
        ostr << ')';
    }

    ostr << " ((b)"
         << ' ' << quantize( disp.show_.ball_.x_, PREC )
         << ' ' << quantize( disp.show_.ball_.y_, PREC );
    if ( disp.show_.ball_.hasVelocity() )
    {
        ostr << ' ' << quantize( disp.show_.ball_.vx_, PREC )
             << ' ' << quantize( disp.show_.ball_.vy_, PREC );
    }
    else
    {
        ostr << " 0 0";
    }
    ostr << ')';

    for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
    {
        const rcss::rcg::PlayerT & p = disp.show_.player_[i];

        ostr << " ("
             << '(' << p.side_ << ' ' << p.unum_ << ')'
             << ' ' << p.type_
             << ' ' << std::hex << std::showbase << p.state_ << std::dec << std::noshowbase;
        ostr << ' ' << quantize( p.x_, PREC )
             << ' ' << quantize( p.y_, PREC );
        if ( p.hasVelocity() )
        {
            ostr << ' ' << quantize( p.vx_, PREC )
                 << ' ' << quantize( p.vy_, PREC );
        }
        else
        {
            ostr << " 0 0";
        }
        ostr << ' ' << quantize( p.body_, DPREC )
             << ' ' << ( p.hasNeck() ? quantize( p.neck_, DPREC ) : 0.0 );
        if ( p.isPointing() )
        {
            ostr << ' ' << quantize( p.point_x_, PREC )
                 << ' ' << quantize( p.point_y_, PREC );
        }

        if ( p.hasView() )
        {
            ostr << " (v " << p.view_quality_ << ' ' << quantize( p.view_width_, DPREC ) << ')';
        }
        else
        {
            ostr << " (v h 90)";
        }

        if ( p.hasStamina() )
        {
            ostr << " (s "
                 << quantize( p.stamina_, 0.001f )<< ' '
                 << quantize( p.effort_, 0.0001f ) << ' '
                 << quantize( p.recovery_, 0.0001f );
            if ( version() >= 4 )
            {
                ostr << ' ' << quantize( p.stamina_capacity_, 0.001f );
            }
            ostr << ')';
        }
        else
        {
            if ( version() >= 4 )
            {
                ostr << " (s 4000 1 1 -1)";
            }
            else
            {
                ostr << " (s 4000 1 1)";
            }
        }

        ostr << " (c "
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
        ostr << ')';
    }
    ostr << ')';

    msg = ostr.str();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
RemoteMonitor::undedicatedRecv( const char * msg,
                                const int len )
{
    if ( M_version >= 2 )
    {
        char err_msg[] = "(error only_init_allowed_on_init_port)";
        send( err_msg, std::strlen( err_msg ) + 1 );
    }
    else
    {
        processMsg( msg, len );
    }

}

/*-------------------------------------------------------------------*/
/*!

 */
void
RemoteMonitor::processMsg( const char * msg,
                           const int len )
{
    std::string str( msg, len );

    if ( ! std::strncmp( str.c_str(), "(dispbye)", std::strlen( "(dispbye)" ) ) )
    {
        M_alive = false;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
RemoteMonitor::handleReceive()
{
    char buf[8192];
    while ( M_socket->hasPendingDatagrams() )
    {
        int n = M_socket->readDatagram( buf, 8192 );

        if ( n > 0 )
        {
            processMsg( buf, n );
        }
    }

    if ( ! M_alive )
    {
        disconnect();
    }
}
