/*
 *Header:
 *File: netif.h (for C++)
 *Author: Noda Itsuki
 *Date: 1996/12/1
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

#ifndef RCSSLOGPLAYER_NETIF_H
#define RCSSLOGPLAYER_NETIF_H

#include "types.h"

#include <rcssbase/net/addr.hpp>
#include <rcssbase/net/udpsocket.hpp>

/*
 *===================================================================
 *Part: Port class
 *===================================================================
 */
class Port {
public:
    char rbuf[256] ;			/* recv buffer */
    displist_t top;


    ~Port();

		void	init();
		void	send_info( const dispinfo_t *, const rcss::net::Addr& dest );
		void	send_info( const dispinfo_t2 *, const rcss::net::Addr& dest );
		int		recv_info();
		void	monitor_init();

    rcss::net::Addr m_listen_addr;
    rcss::net::UDPSocket m_socket;
};

#endif
