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

#ifndef NCCENTRIFUGE_PROPERTY_MANAGER
#define NCCENTRIFUGE_PROPERTY_MANAGER

#include <string>
#ifdef WIN32
	#include <map>
#else
	#include <tr1/unordered_map>
#endif
#include <boost/functional/hash.hpp>
#include "elements/parameter.h"
namespace ncc {
namespace property 
{
    template<class index_type, class property_type>
        class generic_manager
        {
            public:
                generic_manager(){}
                generic_manager(const generic_manager& other) : properties(other.properties) {}
                void set_property(const index_type& index, const property_type& property) { properties[index] = property;}
                property_type get_property(const index_type& index) 
                { 
                    //typename property_map::const_iterator found_property = properties.find(index);
                    //return found_property != properties.end() ? (*found_property) : property_type();
                    return properties[index];
                }
            private:
#ifdef WIN32
                //use map
                typedef std::map<index_type, property_type> property_map;
#else
                //use hash map
                typedef std::tr1::unordered_map<index_type, property_type, boost::hash<index_type> > property_map;
#endif
                property_map properties;
        };

    typedef generic_manager<std::string, parameter> manager;
}//namespace property
}//namespace ncc
#endif

