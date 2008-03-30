// -*-c++-*-

/*!
  \file rcgparser.h
  \brief Class for parsing rcg files
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
               Hidehisa AKIYAMA

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

/////////////////////////////////////////////////////////////////////

#ifndef RCSSLOGPLAYER_RCG_PARSER_H
#define RCSSLOGPLAYER_RCG_PARSER_H

#include <rcsslogplayer/types.h>

#include <iosfwd>
#include <string>

namespace rcss {
namespace rcg {

class Handler;

/*!
  class Parser
  \brief rcg parser
 */
class Parser {
private:
    //! reference to the data handler instance
    Handler & M_handler;

    PlayMode M_playmode;
    TeamT M_teams[2];

    // not used
    Parser();
    Parser( const Parser & );
    Parser & operator=( const Parser & );

    /*!
      \brief construct parser object with data handler class
      \param handler reference to the handler class instance
     */
    explicit
    Parser( Handler & handler );

    /*!
      \brief destructor
     */
    ~Parser()
      { }

    /*!
      \brief analyze rcg data from input stream
      \param is reference to the imput stream
      \return true if successfuly parsed.
     */
    bool parse( std::istream & is );

private:

    bool parseHeader( std::istream & is );

    //
    // version 3 or older
    //

    bool parseLoop( std::istream & is );

    bool parseNext( std::istream & is );
    bool parseDispInfo( std::istream & is );
    bool parseMode( std::istream & is );
    bool parseItem( std::istream & is,
                    const Int16 mode );
    bool parseShowInfo( std::istream & is );
    bool parseMsgInfo( std::istream & is );
    bool parsePlayMode( std::istream & is );
    bool parseTeamInfo( std::istream & is );
    bool parsePlayerType( std::istream & is );
    bool parsePlayerParams( std::istream & is );
		bool parseServerParams( std::istream & is );

    //
    // version 4
    //

    bool parseLines( std::istream & is );

    bool parseShowLine( const int n_line,
                        const std::string & line );
    bool parseDrawLine( const int n_line,
                        const std::string & line );
    bool parseMsgLine( const int n_line,
                       const std::string & line );
    bool parsePlayModeLine( const int n_line,
                            const std::string & line );
    bool parseTeamLine( const int n_line,
                        const std::string & line );
    bool parsePlayerTypeLine( const int n_line,
                              const std::string & line );
    bool parsePlayerParamLine( const int n_line,
                               const std::string & line );
    bool parseServerParamLine( const int n_line,
                               const std::string & line );

    //
    // common
    //

    bool strmErr( std::istream & is );


public:

    static
    void convert( const dispinfo_t & from,
                  DispInfoT & to );
    static
    void convert( const showinfo_t & from,
                  ShowInfoT & to );
    static
    void convert( const short_showinfo_t2 & from,
                  ShowInfoT & to );
    static
    void convert( const team_t & from,
                  TeamT & to );
    static
    void convert( const player_type_t & from,
                  PlayerTypeT & to );
    static
    void convert( const server_params_t & from,
                  ServerParamT & to );
    static
    void convert( const player_params_t & from,
                  PlayerParamT & to );

};

}
}

#endif
