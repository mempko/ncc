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

#include "scripting/script.h"
namespace ncc {
namespace lua {

    script::script() : script_loaded(false), name()
    {
        lua_state = lua_open();
        luabind::open(lua_state); // initialize luabind
        luaL_openlibs(lua_state);   
    }
    script::~script()
    {
        if(lua_state)
        {
            lua_close(lua_state);
            lua_state = 0;
        }
    }
    
    bool script::load(const std::string& file_name)
    {
        if(luaL_dofile(lua_state,file_name.c_str()) == LUA_ERRFILE) return false;
        name = file_name;
        script_loaded = true;
        return true;
    }
} //namespace lua
} //namespace ncc
