// -*-c++-*-

/*!
  \file monitor_server.cpp
  \brief monitor server class Source File.
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

#include "monitor_server.h"

#include "remote_monitor.h"
#include "main_data.h"

#include <rcsslogplayer/types.h>
#include <rcsslogplayer/util.h>

#include <sstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

/*-------------------------------------------------------------------*/
/*!

*/
MonitorServer::MonitorServer( QObject * parent,
                              const MainData & main_data,
                              const int port )

    : QObject( parent )
    , M_main_data( main_data )
    , M_port( static_cast< quint16 >( port ) )
    , M_socket( new QUdpSocket( this ) )

{
    //if ( ! M_socket->bind( QHostAddress::Any, M_port ) )
    if ( ! M_socket->bind( M_port ) )
    {
        std::cerr << "MonitorServer. failed to bind the socket."
                  << std::endl;
        M_socket->close();
        return;
    }

    if ( ! isConnected() )
    {
        std::cerr << "MonitorServer. failed to initialize the socket."
                  << std::endl;
        M_socket->close();
        return;
    }

    // setReadBufferSize() makes no effect for QUdpSocet...
    // M_socket->setReadBufferSize( 8192 * 256 );

    connect( M_socket, SIGNAL( readyRead() ),
             this, SLOT( handleReceive() ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
MonitorServer::~MonitorServer()
{
    if ( isConnected() )
    {
        M_socket->close();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
MonitorServer::isConnected() const
{
    return M_socket
        && M_socket->socketDescriptor() != -1;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MonitorServer::handleReceive()
{
    char buf[8192];
    while ( M_socket->hasPendingDatagrams() )
    {
        QHostAddress addr;
        quint16 from_port;
        int n = M_socket->readDatagram( buf,
                                        8192,
                                        &addr,
                                        &from_port );
        if ( n > 0 )
        {
            bool found = false;
            for ( std::vector< RemoteMonitor * >::iterator it = M_monitors.begin();
                  it != M_monitors.end();
                  ++it )
            {
                if ( (*it)->port() == from_port
                     && (*it)->addr() == addr )
                {
                    // already exist same client.
                    (*it)->undedicatedRecv( buf, n );
                    found = true;
                    break;
                }
            }

            if ( ! found )
            {
                if ( buf[n - 1] == '\n' )
                {
                    --n;
                }
                buf[n] = '\0';
                parseMonitorInit( buf, addr, from_port );
            }
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MonitorServer::parseMonitorInit( const char * msg,
                                 const QHostAddress & addr,
                                 const quint16 port )
{
    double ver = 1.0;
    if ( ! std::strcmp( msg, "(dispinit)" ) )
    {
        RemoteMonitor * mon = new RemoteMonitor( this, 1 );
        if ( ! mon->connect( addr, port ) )
        {
            delete mon;
            return;
        }

        std::cout << "a new (v1) monitor connected" << std::endl;
        M_monitors.push_back( mon );
        sendInit( *mon );
        return;
    }

    if ( std::sscanf( msg, "(dispinit version %lf)", &ver ) == 1 )
    {
        if ( ver < 1.0 || 5.0 <= ver )
        {
            std::cout << "Unsupported monitor protocol version. " << ver
                      << std::endl;
            return;
        }

        // a new monitor connected
        RemoteMonitor * mon = new RemoteMonitor( this, static_cast< int >( ver ) );

        if( ! mon->connect( addr, port ) )
        {
            delete mon;
            return;
        }

        std::cout << "a new (v" << ver << ") monitor connected" << std::endl;
        M_monitors.push_back( mon );

        // send server parameter information & current display data to monitor
        sendInit( *mon );
        return;
    }

}

/*-------------------------------------------------------------------*/
/*!

*/
void
MonitorServer::sendInit( RemoteMonitor & monitor )
{
    // server_param
    // player_param
    // player_type

    if ( monitor.version() == 2 )
    {
        rcss::rcg::dispinfo_t2 disp2;

        disp2.mode = htons( rcss::rcg::PARAM_MODE );
        rcss::rcg::convert( M_main_data.serverParam(), disp2.body.sparams );
        monitor.send( reinterpret_cast< const char * >( &disp2 ),
                      sizeof( rcss::rcg::dispinfo_t2 ) );

        disp2.mode = htons( rcss::rcg::PPARAM_MODE );
        rcss::rcg::convert( M_main_data.playerParam(), disp2.body.pparams );
        monitor.send( reinterpret_cast< const char * >( &disp2 ),
                      sizeof( rcss::rcg::dispinfo_t2 ) );

        disp2.mode = htons( rcss::rcg::PT_MODE );
        for ( std::map< int, rcss::rcg::PlayerTypeT >::const_iterator it = M_main_data.playerTypes().begin();
              it != M_main_data.playerTypes().end();
              ++it )
        {
            rcss::rcg::convert( it->second, disp2.body.ptinfo );
            monitor.send( reinterpret_cast< const char * >( &disp2 ),
                          sizeof( rcss::rcg::dispinfo_t2 ) );
        }
    }
    else if ( monitor.version() >= 3 )
    {
        std::string msg;
        {
            std::ostringstream ostr;
            M_main_data.serverParam().print( ostr );
            msg = ostr.str();

            monitor.send( msg.c_str(), msg.length() + 1 );
        }
        {
            std::ostringstream ostr;
            M_main_data.playerParam().print( ostr );
            msg = ostr.str();

            monitor.send( msg.c_str(), msg.length() + 1 );
        }

        for ( std::map< int, rcss::rcg::PlayerTypeT >::const_iterator it = M_main_data.playerTypes().begin();
              it != M_main_data.playerTypes().end();
              ++it )
        {
            std::ostringstream ostr;
            it->second.print( ostr );
            msg = ostr.str();

            monitor.send( msg.c_str(), msg.length() + 1 );
        }
    }

    // send current display data
    DispConstPtr disp = M_main_data.getDispInfo( M_main_data.index() );
    if ( disp )
    {
        monitor.send( *disp );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MonitorServer::removeDisconnectedClient()
{
    for ( std::vector< RemoteMonitor * >::iterator it = M_monitors.begin();
          it != M_monitors.end();
          )
    {
        if ( ! (*it)->isConnected() )
        {
            std::cerr << PACKAGE <<": remove disconnected client : host="
                  << (*it)->addr().toString().toStdString()
                  << ", port=" << (*it)->port()
                  << std::endl;

            delete *it;
            it = M_monitors.erase( it );
        }
        else
        {
            ++it;
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
MonitorServer::sendToClients()
{
    removeDisconnectedClient();

    if ( M_monitors.empty() )
    {
        return;
    }

    DispConstPtr disp = M_main_data.getDispInfo( M_main_data.index() );
    if ( ! disp )
    {
        return;
    }

    for ( std::vector< RemoteMonitor * >::iterator it = M_monitors.begin();
          it != M_monitors.end();
          ++it )
    {
        (*it)->send( *disp );
    }
}
