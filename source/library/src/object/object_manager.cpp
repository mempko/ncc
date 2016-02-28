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

#include "object/object_manager.h"
namespace ncc {
namespace object 
{
    weak_ptr manager::add_object(ptr object)
    {
        //we do not add a object if the object is null
        if(!object) return ptr();

        //We insert an object and if the insert fails because it already exists we return a 0 pointer.
        std::pair<object_set::iterator, bool> result = objects.insert(object);
        if(result.second == false) return ptr();

        return object;
    }
    
    void manager::remove_object(weak_ptr& object_ptr)
    {			
        object_set::iterator object_to_erase = objects.find(object_ptr.lock());
        if(object_to_erase != objects.end())
            objects.erase(object_to_erase);
    }		
    
    void manager::remove_object(abstract_interface* object_ptr)
    {
        object_set::iterator end = objects.end();
        for(object_set::iterator object_to_erase = objects.begin(); object_to_erase != end; ++object_to_erase)
            if(object_to_erase->get() == object_ptr)
            { 
                objects.erase(object_to_erase);
                break;
            }
    }
    
    void update_object(const ptr& object)
    {
        object->update();
    } 

    void manager::step()
    {
        for_each(objects.begin(), objects.end(), update_object);
    }
}//namespace object
}//namespace ncc
