// -*-c++-*-

/*!
  \file view_holder.h
  \brief view data holder class Header File.
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

#ifndef RCSSLOGPLAYER_DISP_HOLDER_H
#define RCSSLOGPLAYER_DISP_HOLDER_H

#include <rcsslogplayer/types.h>

#include <boost/shared_ptr.hpp>
#include <vector>

typedef boost::shared_ptr< rcss::rcg::DispInfoT > DispPtr;
typedef boost::shared_ptr< const rcss::rcg::DispInfoT > DispConstPtr;

class DispHolder {
private:

    std::vector< DispPtr > M_dispinfo_cont;

    // not used
    DispHolder( const DispHolder & );
    const DispHolder & operator=( const DispHolder & );
public:

    DispHolder();
    ~DispHolder();


    DispConstPtr getDispInfo( const std::size_t idx ) const;

};

#endif
