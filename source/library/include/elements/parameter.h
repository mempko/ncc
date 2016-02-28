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

#ifndef NCCENTRIFUGE_PARAMETER_H
#define NCCENTRIFUGE_PARAMETER_H

#include <boost/any.hpp>
#include <vector>
namespace ncc
{
    ///A holder of any type.
    ///
    ///A holder of any type of object. This is used by the message system to 
    ///send around arbitrary messages. This is simple a typedef of boost::any.
    ///See http://www.boost.org for more documentation on boost::any.
    typedef boost::any parameter;

    ///A list of parameters
    ///
    ///This is not really a list but a vector of parameters. The parameters 
    ///therefore can be accessed with the '[]' index operator. Makes sure to 
    ///check for the amount of parameters to prevent any out of bounds problems. 
    ///The objects which the parameters hold can be easily extracted by using 
    ///ncc::get function.
    ///@see ncc::get
    typedef std::vector<boost::any> parameter_list;

    ///Returns true if a parameter holds an object of a certain type.
    template <class type>
        bool is_type(const parameter & operand)
        {
            return boost::any_cast<type>(&operand);
        }



    ///Retrieves an object of a certain type from a parameter.
    ///
    ///This method retrieves an object of the type specified. If the object 
    ///could not be retrieved, false is returned.\n
    ///Example:
    ///     - get(param[0], name);
    ///
    ///Where param is a ncc::parameter_list and name is a std::string. The 
    ///example retrieves a std::string object from the first parameter. If the 
    ///std::string object could not be retrieved then name is not changed! Note 
    ///the requirements on the second parameter are that it supports the '=' 
    ///operator.
    ///@param operand A parameter which holds an object of the specified type.
    ///@param result This will be equal to the object the parameter.
    ///@return True on success and false on failure to get the object.
    template <class type>
        bool get(const parameter& operand, type& result)
        {
            if(const type* held = boost::any_cast<type>(&operand)) 
            {
                result = *held; 
                return true;
            }
            return false; //error getting object return failure.
        }

    ///Retrieves an object of certain type from a parameter.
    ///
    ///This method retrieves an object of the specified type. If the object 
    ///could not be retrieved, then a default contructed object is returned of 
    ///that type.\n 
    ///Example:
    ///		- std::string name = get<std::string>(param[0]);
    ///
    ///Where the param is a ncc::parameter_list and name is an std::string. 
    ///The requirements of the type is that it is copyable, assignable, and has 
    ///a defualt destructor. 
    ///@param operand A parameter which holds an object of the specified type.
    template<class type>
        type get (const parameter& operand)
        {
            if(const type* held = boost::any_cast<type>(&operand))		
                return *held;				
            else	
                return type();		
        }

    ///Simple wrapper function to create a parameter from an object.
    ///
    ///Since a parameter can hold any kind of object, this function creates
    ///such a parameter from that object. This function is simply a wrapper
    ///and exists for convenience. It is advisable to use the constructor
    ///directly. 
    template<class type>
        parameter create(const type& value)
        {
            return parameter(value);
        }

    ///This function creates a ncc::parameter_list from the parameters given.
    ///
    ///The current maximum amount of parameters are 14. 
    ///The usage is very simple.\n
    /// Example:
    ///     - parameter_list myParameters = parameters("Hello", 42, someObject);
    ///
    ///The example creates a parameter_list with 3 objects. Notice that the 
    ///objects can be of any type which is copyable! This allows arbitrary 
    ///objects to be held in the parameter_list. Use ncc::get function to extract
    ///specfic objects from the parameter list. 
    ///calling "std::string greeting = ncc::get<std::string>(myParameters[0]);" 
    ///will copy the parameter "Hello" to the std::string greeting.

    template <class t1> parameter_list parameters(const t1& v1)
    {
        return parameter_list(1, v1);
    }
    template <class t1, class t2> parameter_list parameters(const t1& v1, const t2& v2)
    {
        parameter_list p; p.reserve(2);
        p.push_back(v1); p.push_back(v2);
        return p;
    }
    template <class t1, class t2, class t3> parameter_list parameters(const t1& v1,const t2& v2,const t3& v3)
    {
        parameter_list p; p.reserve(3);
        p.push_back(v1); p.push_back(v2); p.push_back(v3);
        return p;
    }
    template <class t1, class t2, class t3, class t4> parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4)
    {
        parameter_list p; p.reserve(4);
        p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4);
        return p;
    }
    template <class t1, class t2, class t3, class t4, class t5> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5)
        {
            parameter_list p; p.reserve(5);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6)
        {
            parameter_list p; p.reserve(6);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7)
        {
            parameter_list p; p.reserve(7);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6); p.push_back(v7);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8)
        {
            parameter_list p; p.reserve(8);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6); p.push_back(v7); p.push_back(v8);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8, 
                const t9& v9)
        {
            parameter_list p; p.reserve(9);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6); p.push_back(v7); p.push_back(v8); p.push_back(v9);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8, 
                const t9& v9,const t10& v10)
        {
            parameter_list p; p.reserve(10);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6); p.push_back(v7); p.push_back(v8); p.push_back(v9); p.push_back(v10);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10, class t11> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8, 
                const t9& v9,const t10& v10,const t11& v11)
        {
            parameter_list p; p.reserve(11);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6); p.push_back(v7); p.push_back(v8); p.push_back(v9); p.push_back(v10);
            p.push_back(v11);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10, class t11, class t12> 
        parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8, 
                const t9& v9,const t10& v10,const t11& v11,const t12& v12)
        {
            parameter_list p; p.reserve(12);
            p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
            p.push_back(v6); p.push_back(v7); p.push_back(v8); p.push_back(v9); p.push_back(v10);
            p.push_back(v11); p.push_back(v12);
            return p;
        }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10, class t11, class t12,
             class t13> 
                 parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8, 
                         const t9& v9,const t10& v10,const t11& v11,const t12& v12,const t13& v13)
                 {
                     parameter_list p; p.reserve(13);
                     p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
                     p.push_back(v6); p.push_back(v7); p.push_back(v8); p.push_back(v9); p.push_back(v10);
                     p.push_back(v11); p.push_back(v12); p.push_back(v13);
                     return p;
                 }
    template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10, class t11, class t12,
             class t13,class  t14> 
                 parameter_list parameters(const t1& v1,const t2& v2,const t3& v3,const t4& v4,const t5& v5,const t6& v6,const t7& v7,const t8& v8, 
                         const t9& v9,const t10& v10,const t11& v11,const t12& v12,const t13& v13,const t14& v14)
                 {
                     parameter_list p; p.reserve(14);
                     p.push_back(v1); p.push_back(v2); p.push_back(v3); p.push_back(v4); p.push_back(v5);
                     p.push_back(v6); p.push_back(v7); p.push_back(v8); p.push_back(v9); p.push_back(v10);
                     p.push_back(v11); p.push_back(v12); p.push_back(v13); p.push_back(v14);
                     return p;
                 }
}//namespace ncc
#endif

