/* -*- Mode: C++ -*-
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "logplayer.h"

//#include <rcssbase/version.hpp>

#include <iostream>
#include <csignal>
#include <cstring>
#include <cerrno>

Player player;

namespace {

char Copyright[] = "Copyright (C) 1995 - 1999 Electrotechnical Laboratory.\n\
2000 - 2008 RoboCup Soccer Simulator Maintenance Group.\n";


void
sig_handle( int )
{
    player.quit();
}

}

int
main( int argc, char ** argv )
{
    std::cout << "rcsslogplayer_classic-" << VERSION << "\n\n"
              << Copyright << std::endl;
//     std::cout << "Using rcssbase-" << rcss::base::version()
//               << std::endl << std::endl;


    struct sigaction sig_action;
    sig_action.sa_handler = &sig_handle;
    sig_action.sa_flags = 0;
    if ( sigaction( SIGINT, &sig_action , NULL ) != 0
         || sigaction( SIGTERM, &sig_action , NULL ) != 0
         || sigaction( SIGHUP, &sig_action , NULL ) != 0 )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": could not set signal handler: "
                  << strerror( errno ) << std::endl;
        return 1;
    }

    player.run( argc, argv );

    return 0;
}
