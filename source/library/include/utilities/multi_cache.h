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

#ifndef NCCENTRIFUGE_MULTI_CACHE_H
#define NCCENTRIFUGE_MULTI_CACHE_H

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
    ///Caches values based on a key
    ///
    ///multi_cache is similar to cache except that it can cache multiple values 
	///with the same key. Therefore the get_data method returns a data_range. 
	///The data_range is an std::pair storing the begining and end iterators of 
	///the cached values of a certain key.
    template <class data_type, 
                    class index_type = std::string,
                    template <class> class smart_ptr_type = boost::shared_ptr>
    class multi_cache
    {        
        public:    
            ///can be used to create the proper data ptr to point to the data in the 
            ///multi cache.
            typedef smart_ptr_type<data_type> data_ptr;
#ifdef WIN32
            typedef std::multimap<index_type, data_ptr> multi_cache_map;		
#else		
            typedef std::tr1::unordered_multimap<index_type, data_ptr, boost::hash<index_type> > multi_cache_map;
#endif

            ///A std::pair holding the begining and end iterators of a range of 
            ///cached data.
            typedef std::pair<typename multi_cache_map::iterator, typename multi_cache_map::iterator> data_range;

            multi_cache() {}
            multi_cache(const multi_cache& old) : data_cache(old.data_cache) {}

            ///Returns a range of cached data specified by the identifier parameter.
            data_range get_data(const index_type& identifier);

            ///Caches a piece of data with the specified key. 
            ///
            ///Note that you can add multiple pieces of data with the same key.
            ///To get the data back you use get_data.
            void cache_data(const index_type& identifier, data_ptr data);

            ///Clears the cache of all its data.
            void flush() { data_cache.clear();}
        private:
            multi_cache_map data_cache;                                          
    };

    template <class data_type, class index_type, template <class> class smart_ptr_type>
    typename multi_cache<data_type, index_type, smart_ptr_type>::data_range 
    multi_cache<data_type, index_type, smart_ptr_type>::get_data(const index_type& identifier)
    {
        return this->data_cache.equal_range(identifier);
    }
    template <class data_type, class index_type, template <class> class smart_ptr_type>
    void 
    multi_cache<data_type, index_type, smart_ptr_type>::cache_data(const index_type& identifier, data_ptr data)
    {
        data_cache.insert(data);
    }

}//namespace ncc
#endif

