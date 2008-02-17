/* -*- Mode: C++ -*-
 *Header:
 *File: netif.C (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1996/02/14
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "netif.h"

#include "param.h"
#include "types.h"
#include "logplayer.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>

/*
 *===================================================================
 *  UDP Send
 *===================================================================
 */

Port::~Port()
{
    displist_t * p = top.next;
    while ( p != NULL )
    {
        displist_t * prev = p;
        p = p->next;

        delete prev;
    }
}

void
Port::init()
{
    //[2004-09-24:I.Noda]
    // Addr can not accept sting address like "localhost".
    // This argument should be not specified for normal use, I think.

    //    m_listen_addr = rcss::net::Addr( DEFAULT_PORT_NUMBER, "localhost");
    m_listen_addr = rcss::net::Addr( DEFAULT_PORT_NUMBER ) ;

    m_socket.setNonBlocking();

    m_socket.bind( m_listen_addr );

    top.next = NULL;
}

void
Port::send_info( const dispinfo_t * msg,
                 const rcss::net::Addr & dest )
{
    m_socket.send( reinterpret_cast< const char * >( msg ),
                   sizeof( dispinfo_t ),
                   dest );
}

void
Port::send_info( const dispinfo_t2 * msg,
                 const rcss::net::Addr & dest )
{
    m_socket.send( reinterpret_cast< const char * >( msg ),
                   sizeof( dispinfo_t2 ),
                   dest );
}

int
Port::recv_info()
{
    rcss::net::Addr from;
    int n = m_socket.recv( rbuf, sizeof( rbuf ), from );

    if ( n <= 0 )
    {
        return n;
    }

    int ver = 0;
    if ( std::sscanf( rbuf, "(dispinit version %d)", &ver ) == 1 )
    {
        //p->next->version = ver;
    }
    else if ( ! std::strncpy( rbuf, "(dispinit)", std::strlen( "(dispinit)" ) ) )
    {
        ver = 1;
        //p->next->version = 1;
    }
    else if ( ! std::strncpy( rbuf, "(dispbye)", std::strlen( "(dispbye)" ) ) )
    {
        std::cout << "a monitor disconnected" << std::endl;
        return n;
    }

    if ( ver == 0 )
    {
        return n;
    }

    displist_t * p = &top;
    while ( p->next != NULL )
    {
        p = p->next;
    }

    p->next = new displist_t;
    p->next->m_addr = from;
    p->next->version = ver;

    std::cout << "a new (v" << ver << ") monitor connected" << std::endl;

    return n;
}
