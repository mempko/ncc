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

#ifndef NCCENTRIFUGE_CACHE_H
#define NCCENTRIFUGE_CACHE_H
#include <string>
#ifdef WIN32
	#include <map>
#else
	#include <tr1/unordered_map>
#endif
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
namespace ncc
{

    ///Generic cache class which does simple data management.
    ///
    ///To cache something is to store it away in hiding for a future use. This 
    ///is exactly what the cache class does. You create some data and put it in 
    ///the cache. This data has a name or value associated with it. Whenever you 
    ///need that data again for use you retrieve it by calling get_data giving 
    ///it that name or id. This idea is very useful for sharing resources across 
    ///multiple objects as those objects can have a pointer pointing to the 
    ///cached data. An example is lets say you have several 3d objects using the 
    ///same image texture. You can store this image texture in the cache with 
    ///the identifier being the file name and have those 3d objects hold a
    ///pointer into the cache.
    ///\n
    ///There three template parameters are
    ///     - data_type represents the type of the data you want to cache
    ///     - index_type represents the type you want to use to retrieve the
    ///		  data. default is std::string
    ///     - smart_ptr_type represents the smart pointer type you want to use. 
    ///		  default is boost::shared_ptr
    template <class data_type, 
             class index_type = std::string,
             template <class> class smart_ptr_type = boost::shared_ptr>
                 class cache
                 {        
                     public:    
                         ///can be used to create the proper data ptr to point to the data in the 
                         ///cache.
                         typedef smart_ptr_type<data_type> data_ptr;
                         cache() {}
                         cache(const cache& old) : data_cache(old.data_cache) {};

                         ///returns a pointer to some data in the cache.
                         ///
                         ///@param identifier some identifier refering to the data in the cache. 
                         ///The type of the identifier is the same type as specified in the 
                         ///index_type template parameter
                         data_ptr get_data(const index_type& identifier);

                         ///Caches the data.
                         ///
                         ///To cache something is to put it away for future use. This method 
                         ///caches some data with a certain identifier into the data cache. 
                         ///retrieve the data with that identifier by calling 
                         ///get_data(identifier).
                         ///@param identifier The identifier is a way which we can refer to the 
                         ///data we want to cache.
                         ///@param data_ptr A pointer to the data we want to cache. Note the 
                         ///pointer is a smart pointer of the type.
                         ///specified by the template parameter smart_ptr_type
                         void cache_data(const index_type& identifier, data_ptr data);

                         ///Clears the cache of all the data
                         ///
                         ///This clears the cache of all the data. If the smart pointer type used 
                         ///is for instance the defualt boost::smart_ptr type provided any other 
                         ///reference counted smart pointer then any object which holds a pointer 
                         ///to the data in the cache will still have that data available to it 
                         ///even though the cache is empty. This is the advantage of a smart 
                         ///pointer. The reason flush exists is that we may need to clear the 
                         ///cache from time to time, for instance, between level loads in a game.
                         void flush() { data_cache.clear();}
                     private:
#ifdef WIN32
                         ///Internally the cache is stored as a map.
                         typedef std::map<index_type, data_ptr> cache_map;
#else
                         ///Internally the cache is stored as a hash map.
                         typedef std::tr1::unordered_map<index_type, data_ptr, boost::hash<index_type> > cache_map;
#endif
                         cache_map data_cache;                                          
                 };

    template <class data_type, class index_type, template <class> class smart_ptr_type>
        typename cache<data_type, index_type, smart_ptr_type>::data_ptr 
        cache<data_type, index_type, smart_ptr_type>::get_data(const index_type& identifier)
        {
            typename cache_map::iterator cached_data = this->data_cache.find(identifier);
            return cached_data != this->data_cache.end() ? cached_data->second : data_ptr();
        }
    template <class data_type, class index_type, template <class> class smart_ptr_type>
        void 
        cache<data_type, index_type, smart_ptr_type>::cache_data(const index_type& identifier, data_ptr data)
        {
            data_cache[identifier] = data;
        }

}//namespace ncc
#endif

