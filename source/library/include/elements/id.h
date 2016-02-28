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

#ifndef NCCENTRIFUGE_ID_H
#define NCCENTRIFUGE_ID_H
namespace ncc 
{
    ///A class that generates IDs for an object.
    ///
    ///This class is used to generate id_type values for an object. 
    ///This class is sortable and comparable to other id_type objects. Can be 
    ///used in a STL container
    class id_type
    {
        public:
            ///The only constructor of the id_type class.
            ///
            ///Generates an unique id_type value for any class which inherits from 
            ///the id_type class. The id_type is constant and cannot be changed
            id_type() {create_id();}

            ///create a copy of an id_type
            ///
            ///The copy constructor actually just creates a new id. The makes sure
            ///that the id is unique in the application. 
            id_type(const id_type&) {create_id();}
            operator unsigned long() const {return value;}

            ///Assigns one id to the other
            const id_type& operator = (const id_type& rhs) {create_id(); return *this;}

            ///Returns the value of the id.
            unsigned long get_id() const {return value;}

            ///Returns true if the left hand side id_type is less than the right 
            ///hand side id_type.
            bool operator < (const id_type& rhs) const { return value < rhs.value; } 

            ///Returns true if two id values are the same. 
            bool operator ==(const id_type& rhs) const { return value == rhs.value;}

            ///Returns true if the id is equal to the right hand side value
            bool operator ==(unsigned long rhs) const {return value == rhs;}

            ///Returns true if two id values are not the same.
            bool operator != (const id_type& rhs) const {return value != rhs.value;}

        private:
            void create_id() {value=previous_id;previous_id++;}
            ///Generates an id from the previous one. This is a simply the previous 
            ///id plus one. 
            static unsigned long previous_id;	///< this holds the previous id 
            ///value assigned.
            unsigned long value;	///< this holds the actual value of the id_type

    };

    bool operator == (unsigned long lhs, id_type rhs);
} //namespace ncc
#endif

