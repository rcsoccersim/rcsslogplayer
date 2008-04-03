// -*-c++-*-

/*!
	\file disp_holder.cpp
	\brief display data holder class Source File.
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

#include "disp_holder.h"

/*-------------------------------------------------------------------*/
/*!

*/
DispHolder::DispHolder()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
DispHolder::~DispHolder()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
DispConstPtr
DispHolder::getDispInfo( const std::size_t idx ) const
{
    if ( M_dispinfo_cont.size() <= idx )
    {
        return DispConstPtr(); // null pointer
    }

    return M_dispinfo_cont[idx];
}
