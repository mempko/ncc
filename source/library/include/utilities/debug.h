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

#ifndef NCCENTRIFUGE_DEBUG_H
#define NCCENTRIFUGE_DEBUG_H

#include <string>
#include <iostream>
#ifdef NDEBUG 
	#define DEBUG false
#else
	#define DEBUG true 
#endif
namespace ncc
{
    ///Simple class to print debug messages
    ///
    ///Prints simple debug messages to std::cerr. The use is
    ///debug_message<DEBUG>("message"); where DEBUG is a defined to be true or 
    ///false. logically if DEBUG is true then when the source is compiled 
    ///messages are printed. if it is false then nothing happens. A good 
    ///compiler in theory should be able to optimize out the do nothing version.
    template <bool debug> struct debug_message;
    template<> struct debug_message<false>
    {
        debug_message(const std::string& message) {} //do nothing...
    };
    template<> struct debug_message<true>
    {
        debug_message(const std::string& message){std::cerr << message << std::endl;}
    };

}//namespace ncc
#endif

