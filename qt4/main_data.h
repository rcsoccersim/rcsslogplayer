// -*-c++-*-

/*!
	\file main_data.h
	\brief main data class Header File.
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

#ifndef RCSSLOGPLAYER_MAIN_DATA_H
#define RCSSLOGPLAYER_MAIN_DATA_H

#include "disp_holder.h"

#include <ostream>

class QString;
class QWidget;

class MainData {
private:

    DispHolder M_disp_holder;
    std::size_t M_index;

    std::ostream * M_out;
    bool M_record_mode;
    rcss::rcg::PlayMode M_record_playmode;
    rcss::rcg::TeamT M_record_team[2];

    // not used
    MainData( const MainData & );
    const MainData & operator=( const MainData & );
public:

    MainData();
    ~MainData();

    void clear();

    std::size_t index() const
      {
          return M_index;
      }

    DispHolder & dispHolder()
      {
          return M_disp_holder;
      }
    const
    DispHolder & dispHolder() const
      {
          return M_disp_holder;
      }

    DispConstPtr getDispInfo( const std::size_t idx ) const
      {
          return M_disp_holder.getDispInfo( idx );
      }

    const
    rcss::rcg::ServerParamT & serverParam() const
      {
          return M_disp_holder.serverParam();
      }

    const
    rcss::rcg::PlayerParamT & playerParam() const
      {
          return M_disp_holder.playerParam();
      }

    const
    std::map< int, rcss::rcg::PlayerTypeT > & playerTypes() const
      {
          return M_disp_holder.playerTypes();
      }

    const
    rcss::rcg::PlayerTypeT & playerType( const int id ) const
      {
          return M_disp_holder.playerType( id );
      }


    bool openRCG( const QString & file_path,
                  QWidget * parant );

    bool openOutputFile( const QString & file_path );
    void setEnableRecord( bool checked );
    void outputCurrentData();
    void closeOutputFile();

private:
    void serializeShow( std::ostream & os,
                        const rcss::rcg::DispInfoT & disp );

public:

    bool setIndexFirst();
    bool setIndexLast();

    bool setIndexStepBack();
    bool setIndexStepForward();

    bool setIndex( const int index );
    bool setCycle( const int cycle );

};

#endif
