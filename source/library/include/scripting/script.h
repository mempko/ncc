/*
 * Copyright (C) 2016  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give 
 * permission to link the code of portions of this program with the 
 * Botan library under certain conditions as described in each 
 * individual source file, and distribute linked combinations 
 * including the two.
 *
 * You must obey the GNU General Public License in all respects for 
 * all of the code used other than Botan. If you modify file(s) with 
 * this exception, you may extend this exception to your version of the 
 * file(s), but you are not obligated to do so. If you do not wish to do 
 * so, delete this exception statement from your version. If you delete 
 * this exception statement from all source files in the program, then 
 * also delete it here.
 */

#ifndef NCCENTRIFUGE_SCRIPT_H
#define NCCENTRIFUGE_SCRIPT_H

#include "luabind/error.hpp"

extern "C" 
{
#include "lua.h"    
#include "lualib.h"    
#include "lauxlib.h"    
}

/*
#undef check
*/
#include "luabind/luabind.hpp"
#include <string>
#include <iostream>

namespace ncc {
namespace lua 
{
    ///This class wraps a lua script virtual machine.
    ///
    ///Each script that is loaded has its own lua state.
    ///This class wraps the state.
    class script
    {
        public:
            ///Constructor
            script();

            ///Loads a script from the specified file.
            ///
            ///Loads a script from the specified file and runs any code not in
            ///a function.
            bool load(const std::string& file_name);

            ///Returns true if the script has been loaded.
            bool ready() const {return script_loaded;}

            ///Returns a pointer to the lua state.
            ///@{
            lua_State* state(){return lua_state;}
            const lua_State* state() const {return lua_state;}
            ///@}

            ///Returns the file name of the script.
            const std::string& file_name() const {return name;}
            ~script();
        private:
            bool script_loaded;
            lua_State* lua_state;
            std::string name;
    };

}//namespace lua
}//namespace ncc
#endif

