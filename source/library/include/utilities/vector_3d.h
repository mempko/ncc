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

#ifndef NCCENTRIFUGE_VECTOR3D_H
#define NCCENTRIFUGE_VECTOR3D_H

#include <algorithm>
#include <cmath>
//#include "utilities/fixed.h"
namespace ncc 
{
    ///A Template 3D vector class .
    ///
    ///A 3D vector class which use can specify the precious of the v via the 
    ///template parameter value_type. This class overloads many operators so 
    ///that vector math looks logical.\n
    ///It is recommended that your project uses the types typedefed instead of 
    ///the raw template class.
    ///        - vector_3dd has double precision.
    ///        - vector_3df has floating point precision.
    ///        - vector_3di uses fixed point math.
    ///        
    ///This class is not SIMD optimized because I did not have access to a 
    ///Pentium developing this project. If the system which the game is running 
    ///has poor floating point performance, the generic vector_3di class might 
    ///be a good choice.
    template <typename value_type>
        class vector_3d
        {
            public:
                static const unsigned int size = 3;

                ///Standard Constructor.
                vector_3d();  

                ///Copy Constructor.
                template<class other_value_type>
                    vector_3d(const vector_3d<other_value_type>& vec);

                ///Equal Operator.
                template<class other_value_type>
                    const vector_3d& operator = (const vector_3d<other_value_type>& vec); 


                ///Create a vector from single values or x, y, and z.
                vector_3d(value_type x_value, value_type y_value, value_type z_value); 

                ///Sets the vector components manually
                void set(value_type x_value, value_type y_value, value_type z_value); 

                ///Returns the size/length of the 3d vector.
                ///@return sqrt(x*x + y*y + z*z) where x, y, and z are the vector 
                ///values.
                value_type length() const;

                ///Returns the size/length of the 3d vector squared.
                ///@return (x*x + y*y + z*z) where x, y, and z are the vector values.
                value_type length_squared() const;

                ///Adds a vector to the current vector and returns the result.
                ///@param rhs The right hand side vector of the equation 
                ///"vector1 + vector2".
                ///@return the result of "vector1 + vector2".
                vector_3d operator + (const vector_3d& rhs) const;

                ///Subtracts a vector from the current vector and returns the result.
                ///@param rhs The right hand side vector of the equation 
                ///"vector1 - vector2".
                ///@return the result of "vector1 - vector2".
                vector_3d operator - (const vector_3d& rhs) const;

                ///Adds a value to all of the components of the vector and returns the 
                ///result.\n
                ///An example is "vector + 3" which is essentially:
                ///		- temp.x = vector.x + 3\n
                ///		- temp.y = vector.y + 3\n
                ///		- temp.z = vector.z + 3\n
                ///
                ///@return The result of "vector + number".
                vector_3d operator + (value_type rhs) const;

                ///Subtracts a value from all of the components of the vector and 
                ///returns the result.\n
                ///An example is "vector - 3" which is essentially:
                ///		- temp.x = vector.x - 3\n
                ///		- temp.y = vector.y - 3\n
                ///		- temp.z = vector.z - 3\n
                ///
                ///@return A vector_3d of a vector minus a number.
                vector_3d operator - (value_type rhs) const;

                ///Calculates the Dot Product between two vectors and returns the result
                ///as a value.
                ///
                ///Returns the result of "vector1 dot vector2". The dot product formula 
                ///is "x1*x2 + y1*y2 + z1*z2" where x1, y1, and z1 are the respective 
                ///components of vector1 and x2, y2, and z2 are the respective 
                ///components of vector2.
                ///@return A value_type representing the result of the dot product.
                value_type operator * (const vector_3d& rhs) const;

                ///Calculates the Dot product between two vectors.
                ///@see value_type operator * (const vector_3d& rhs) const;
                value_type dot(const vector_3d& rhs) const { return *this * rhs;}

                ///Calculates the Cross product between two vectors and returns the 
                ///result as a vector_3d.
                ///
                ///Returns the result of "vector1 cross vector2". The cross product is 
                ///useful for determining normals. It returns a vector orthogonal to two 
                ///other vectors. The cross product formula is as follows:\n
                ///		- product.x = v.y*w.z - v.z*w.y;\n
                ///		- product.y = v.z*w.x - v.x*w.z;\n
                ///		- product.z = v.x*w.y - v.y*w.x;\n
                ///
                ///Where v and w are vectors.
                ///@return A vector_3d orthogonal to the two vectors involved in the 
                ///cross product.
                vector_3d cross(const vector_3d& rhs) const;

                ///Multiplies a value to the vector and returns the result.
                ///
                ///Multiplies a value against all of the vector components and returns 
                ///the result. This can be used to scale a vector. example: 
                ///"new_vector = vector * 4;" 
                ///"new_vector" will be four times the length of "vector".
                ///@return A vector scaled by a value.
                vector_3d operator * (value_type rhs) const;

                ///Divides all vector components by the value on the right hand side.
                ///
                ///Divides all the vector components bv the value in the right hand 
                ///side. "vector / value". An example of its usage is the ability to 
                ///scale the vector. "vector / 2" would return a vector with half the 
                ///length.
                ///@return The scaled vector in "vector / value".
                vector_3d  operator /  (value_type rhs) const;

                ///Optimization for "vector = vector + rhs".
                void operator +=(const vector_3d& rhs);

                ///Optimization for "vector = vector - rhs".
                void operator -= (const vector_3d& rhs);

                ///Optimization for "vector = vector + rhs".
                void operator +=(value_type rhs);

                ///Optimization for "vector = vector - rhs".
                void operator -= (value_type rhs);

                ///Optimization for "vector = vector * rhs".
                void operator *= (value_type rhs);

                ///Optimization for "vector = vector / rhs".
                void operator /= (value_type rhs);

                ///Creates a normalized vector and returns the result.
                ///
                ///This creates a vector with length one (normalized) and returns it.
                ///The formula is "vector_3d normalized = vector / vector.size()"
                ///@return The a normalized vector from the current vector.
                vector_3d normalized() const;

                ///Normalizes the vector.
                ///
                ///Scales the vector so that its length is one. The formula is
                ///"vector = vector / vector.size();
                void normalize();

                ///Returns a reference to the X value of the vector.
                value_type& x() { return v[0];}
                const value_type& x() const { return v[0];}

                ///Returns a reference to the Y value of the vector.
                value_type& y() { return v[1];}
                const value_type& y() const { return v[1];}

                ///Returns a reference to the Z value of the vector.
                value_type& z() { return v[2];}
                const value_type& z() const { return v[2];}

                value_type get_x() const { return v[0];}
                void set_x(value_type value) {v[0] = value;}

                value_type get_y() const { return v[1];}
                void set_y(value_type value) {v[1] = value;}

                value_type get_z() const { return v[2];}
                void set_z(value_type value) {v[2] = value;}

                ///Returns a const pointer to the raw vector data.
                const value_type* raw_vector() const { return v;}

                ///Returns a pointer to the raw vector data;
                value_type* raw_vector() { return v;}
            private:	 
                value_type v[3];  

        };

    ///A 3D vector with floating point precision.
    ///@see vector_3d
    typedef vector_3d<float> vector_3df;

    ///A 3D vector with double precision.
    ///@see vector_3d
    typedef vector_3d<double> vector_3dd;

    ///A 3D vector class with long double precision.
    ///@see vector_3d
    typedef vector_3d<long double> vector_3dld;
    
    ///A 3D vector class using fixed number precision.
    ///@see vector_3d
//    typedef vector_3d<fixed_long8> vector_3di;

    //Implemination

    template <typename value_type>
        inline vector_3d<value_type>::vector_3d()
        {
            v[0] = v[1] = v[2] = 0;
        }

    template<typename value_type>
        template<class other_value_type>
        inline vector_3d<value_type>::vector_3d(const vector_3d<other_value_type>& vec) 
        {
            *this = vec;
        }

    template<typename value_type>
        template<class other_value_type>
        inline const vector_3d<value_type>& 
        vector_3d<value_type>::operator = (const vector_3d<other_value_type>& vec)
        { 
            std::copy(vec.raw_vector(), vec.raw_vector() + 3, v); 
            return *this;
        }

    template<typename value_type>
        inline vector_3d<value_type>::vector_3d(value_type x_value, value_type y_value, value_type z_value)
        { 
            set(x_value, y_value, z_value);
        }
    template <typename value_type>
        inline void
        vector_3d<value_type>::set(value_type x_value, value_type y_value, value_type z_value)
        { 
            v[0] =  x_value; 
            v[1] =  y_value; 
            v[2] =  z_value;
        }


    template <typename value_type>
        inline value_type 
        vector_3d<value_type>::length() const
        {
            return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }

    template <typename value_type>
        inline value_type 
        vector_3d<value_type>::length_squared() const
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }

    template <typename value_type>
        inline vector_3d<value_type> 
        vector_3d<value_type>::operator + (const vector_3d& rhs) const
        {
            return vector_3d(v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2]);
        }

    template <typename value_type>
        inline vector_3d<value_type> 
        vector_3d<value_type>::operator - (const vector_3d& rhs) const
        {
            return vector_3d(v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2]);
        }

    template <typename value_type>
        inline vector_3d<value_type> 
        vector_3d<value_type>::operator + (value_type rhs) const
        {
            return vector_3d(v[0] + rhs, v[1] + rhs, v[2] + rhs);
        }

    template <typename value_type>
        inline vector_3d<value_type> 
        vector_3d<value_type>::operator - (value_type rhs) const
        {
            return vector_3d(v[0] - rhs, v[1] - rhs, v[2] - rhs);
        }

    template<typename value_type>
        inline value_type 
        vector_3d<value_type>::operator * (const vector_3d& rhs) const
        {
            return v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2];
        }

    template<typename value_type>
        inline vector_3d<value_type> 
        vector_3d<value_type>::cross(const vector_3d& rhs) const
        {
            //This is the cross product fromula.
            //x = v.y*w.z - v.z*w.y;
            //y = v.z*w.x - v.x*w.z;
            //z = v.x*w.y - v.y*w.x;
            //where v and w are vectors
            return vector_3d(v[1]*rhs.v[2] - v[2]*rhs.v[1],   //calculate x value
                    v[2]*rhs.v[0] - v[0]*rhs.v[2],   //calculate y value
                    v[0]*rhs.v[1] - v[1]*rhs.v[0]);  //calculate z value
        }


    template<typename value_type>
        inline vector_3d<value_type>
        vector_3d<value_type>::operator /  (value_type rhs) const
        {
            return vector_3d(v[0] / rhs, v[1] / rhs, v[2] / rhs);
        }

    template <typename value_type>
        inline vector_3d<value_type>
        vector_3d<value_type>::operator * (value_type rhs) const
        {
            return vector_3d(v[0] * rhs, v[1] * rhs, v[2] * rhs);
        }

    template <typename value_type>
        inline vector_3d<value_type> 
        vector_3d<value_type>::normalized() const
        {
            return vector_3d(*this / length());
        }
    template <typename value_type>
        inline void
        vector_3d<value_type>::normalize()
        {
            *this = *this / length();
        }

    template <typename value_type>
        inline void 
        vector_3d<value_type>::operator +=(const vector_3d& rhs)
        {
            v[0] += rhs.v[0];
            v[1] += rhs.v[1];
            v[2] += rhs.v[2];
        }

    template <typename value_type>
        inline void 
        vector_3d<value_type>::operator -= (const vector_3d& rhs)
        {
            v[0] -= rhs.v[0];
            v[1] -= rhs.v[1];
            v[2] -= rhs.v[2];
        }

    template <typename value_type>
        inline void 
        vector_3d<value_type>::operator +=(value_type rhs)
        {
            v[0] += rhs;
            v[1] += rhs;
            v[2] += rhs;
        }

    template <typename value_type>
        inline void 
        vector_3d<value_type>::operator -= (value_type rhs)
        {
            v[0] -= rhs;
            v[1] -= rhs;
            v[2] -= rhs;
        }

    template <typename value_type>
        inline void 
        vector_3d<value_type>::operator *= (value_type rhs)
        {
            v[0] *= rhs; 
            v[1] *= rhs; 
            v[2] *= rhs; 
        }

    template <typename value_type>
        inline void 
        vector_3d<value_type>::operator /= (value_type rhs)
        {
            v[0] /= rhs; 
            v[1] /= rhs; 
            v[2] /= rhs; 
        }
}//namespace ncc
#endif

