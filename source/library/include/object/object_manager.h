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

#ifndef NCCENTRIFUGE_OBJECT_MANAGER_H
#define NCCENTRIFUGE_OBJECT_MANAGER_H
#include <algorithm>
#include <set>
#include <map>
#include <boost/utility.hpp>
#include "object/object_interface.h"
namespace ncc {
namespace object 
{
    typedef std::set<ptr> object_set;
    ///Manages 3d objects.
    ///
    ///The advantage of using the object manager is that all objects
    ///can be updated at once by the manager instead of having to update each
    ///object manually. Further more, all objects are located in one place
    ///and therefore are accessible everywhere.
    ///@ingroup managers
    class manager : boost::noncopyable
    {
        public:                

            ///Adds an object to the object manager.
            ///
            ///This method will add the object to the manager. If the object
            ///is already in the manager, then nothing happens. A boost::weak_ptr 
            ///point to the object is returned. It is recommened you hold a 
            ///weak_ptr instead of the shared_ptr so that delocation of an object
            ///is predictable.
            weak_ptr add_object(ptr object_ptr);

            ///Removes a specific object from the object manager.
            ///
            ///It is important to node that the weak_ptr to the object will not be 
            ///valid after this call.
            //@{
            void remove_object(weak_ptr& object);
            void remove_object(abstract_interface* object_ptr);
            //@}


            ///Returns the begining iterator of the object list.
            ///@{
            object_set::const_iterator begin() const { return objects.begin();}
            object_set::iterator begin() { return objects.begin();}
            ////@}

            ///Returns the end iterator of the object list
            ///@{
            object_set::const_iterator end() const {return objects.end();}
            object_set::iterator end() {return objects.end();}
            ///@}

            ///Returns the amount of objects being managed.
            object_set::size_type size() const {return objects.size();}

            ///Updates every managed object.
            ///
            ///The all objects update methods are called.
            void step();

            manager(){}
        private:
            object_set objects;
    };
}//namesepace object
}//namespace ncc
#endif

