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

#ifndef NCCENTRIFUGE_QUATERNION
#define NCCENTRIFUGE_QUATERNION
#include "utilities/vector_3d.h"
namespace ncc 
{
    ///A Template Quaternion Class.
    ///
    ///This is obviously a template quaternion class. The template parameter 
    ///"value_type" specifies the precision of the particular quaternion. This 
    ///class is very useful for smooth interpolation between orientations in 3d 
    ///space. Iterpolating between rotations using Euler angles produces strange
    ///transformations and jumping movement. Another problem with Euler 
    ///interpolation and rotation is the infamous "Gimbal Lock" where rotating 
    ///90 degrees on one axis, you loose a degree of freedom. Type in 
    ///"Gimbal Lock" in google to find a decent description. Quaternions 
    ///overcome all of these problems and allow for a much smoother experience. 
    ///There are several useful typedefs that are provided for convenience:
    ///		- quaternionf This is a quaternion with float precision.
    ///		- quaterniond This is a quaternion with double precision.
    ///		- quaternioni This is a quaternion with fixed point precision. Use 
    ///		  this one if the hardware does not have a FPU. 
    ///
    ///Remember that quaternion operations are not communitive like matrix 
    ///multiplication. So "quaternion1 * quaternion2" is not the same as 
    ///"quaternion2 * quaternion1".
    template<typename value_type>
        class quaternion
        {
            public:
                ///Standard constructor initializes all values to 0.
                quaternion();

                ///Initializes the quaternion with specific x, y, z, and w values.
                quaternion(value_type x, value_type y, value_type z, value_type w);

                ///Sets the quaternion values manually
                void set(value_type x, value_type y, value_type z, value_type w);

                ///Initializes the quaternion with another quaternion.
                template <class other_value_type>
                    quaternion(const quaternion<other_value_type>& quat);

                ///Sets the quaternion equal to the quaternion on the right hand 
                ///side.
                ///@param rhs The quaternion on the right hand side.
                ///@return A const reference to the quaternion.
                template <typename other_value_type>
                    const quaternion& operator = (const quaternion<other_value_type>& rhs);

                ///Creates a transformation matrix from the quaternion.
                ///
                ///Creates a transformation matrix from the quaternion to be used to 
                ///transform points. Just pass a reference of the matrix which will 
                ///hold the transformation and you are set. The matrix must support  
                ///[][] operations where the martix is a 4x4 size matrix.
                ///@param matrix This is a refrence of the matrix which you want to 
                ///change to a transformation matrix derived from the quaternion.
                template<class matrix_type>
                    void create_matrix(matrix_type& matrix);

                ///Creates a quaternion to represent a rotation using euler angles.
                ///
                ///Creates a quaternion to represent a rotation using euler angles. 
                ///Use this to create a quaternion based on rotations around the 
                ///global x, y, and z axis. You can roll, pitch, and yaw.
                ///		- roll: rotation around x axis.
                ///		- pitch: rotation around y axis.
                ///		- yaw: rotation around z axis.
                ///
                ///The rotations are performed in the following rotation order:
                ///		- yaw -> pitch -> roll
                void create_from_euler(value_type roll, value_type pitch, value_type yaw);

                ///Converts a matrix representation of a rotation to a quaternion.
                ///
                ///Converts a matrix representation of a rotation to a quaternion. 
                ///Remember that the matrix must be in (OpenGL) column major order.
                template <class matrix_type>
                    void create_from_matrix(const matrix_type& matrix);

                ///Returns the x value of the quaternion
                value_type& x() {return v[0];}
                const value_type& x() const { return v[0];}

                ///Returns the y value of the quaternion
                value_type& y() { return v[1];}
                const value_type& y() const {return v[1];}

                ///Returns the z value of the quaternion
                value_type& z() { return v[2];}
                const value_type& z() const {return v[2];}

                ///Returns the w value of the quaternion
                value_type& w() { return v[3];}
                const value_type& w() const {return v[3];}

                value_type get_x() const { return v[0];}
                void set_x(value_type value) {v[0] = value;}

                value_type get_y() const { return v[1];}
                void set_y(value_type value) {v[1] = value;}

                value_type get_z() const { return v[2];}
                void set_z(value_type value) {v[2] = value;}

                value_type get_w() const { return v[3];}
                void set_w(value_type value) {v[3] = value;}

                ///Normalizes the quaternion.
                ///
                ///Normalizes the quaternion so that its length is 1. In other words 
                ///(x^2 + y^2 + z^2 + w^2 = 1). This method essentially makes a 
                ///quaternion a unit quaternion.
                void normalize();

                ///Returns a Normalized version of the quaternion.
                ///
                ///Normalizes the quaternion so that its length is 1 and returns the 
                ///result. In other words (x^2 + y^2 + z^2 + w^2 = 1). This method 
                ///essentially makes a quaternion a unit quaternion. So use this 
                ///method to get a unit version of a quaternion without modifying 
                ///the original.
                quaternion normalized() const;

                ///Dissasembles a quaternion into an axis and angle representation.
                ///
                ///Use this method to extract the axis and angle of rotation which 
                ///the quaternion represents.
                ///@param radians This is a reference to the value you would like to 
                ///hold the angle in radians.
                ///@return The axis of which the quaternio represents.
                vector_3d<value_type> get_axis_angle(value_type& radians);

                ///Creates a quaternion from an axis and angle representation.
                ///
                ///Use this to create a quaternion using an axis and angle to set 
                ///the orienation.
                ///@param axis The axis on which to rotate as a vector_3d
                ///@param radians The angle of rotation on the axis in radians.
                void create_from_axis_angle(vector_3d<value_type> axis, value_type radians);

                ///Returns an inverse quaternion
                ///
                ///Inverts the rotation of the quaternion and returns the result. 
                ///This is equivalent mathimaticaly as Q^(-1). The original 
                ///quaternion is not modified.
                ///@return The inverse quaternion.
                quaternion get_inverse() const;

                ///Multiplies two quaternions together.
                ///
                ///Multiplies the quaternion on the right hand side with the left 
                ///and returns the result. Remember that quaternion multiplication 
                ///is not communitive. This means "q1 * q2" does not equal 
                ///"q2 * q1". Note this is not the same as a vector dot product. To 
                ///do a dot product on two quaternions you must use the 
                ///"value_type dot (const quaternion&);" method. Note that vector 
                ///multiplication is the same as a dot product but quaternion 
                ///multiplication is not.
                ///@return Returns a normalized quaternion since all useful 
                ///quaternions need to be unit.
                quaternion operator * (const quaternion& rhs) const;

                ///Mutliplies a vector by a quaterion
                ///
                ///Multiplying a vector by a quaterion results in a vector which is 
                ///an offset rotation from original vector. This is useful for say 
                ///finding out a pointing vector of a character in a game. You would 
                ///multiply the characters original pointing position by the 
                ///quaternion to get a new vector of the characters facing 
                ///direction.
                vector_3d<value_type> operator * (const vector_3d<value_type>& rhs) const;

                ///Mutliplies two quaternions together.
                ///
                ///Multiplies the quaternion on the right hand side with the left 
                ///where the left one will contain the result. This is a shortcut 
                ///for the lazy man. 
                ///@see quaternion operator *(const quaternion& rhs) const;
                void operator *=(const quaternion& rhs);

                ///Adds two quaternions together.
                ///@return The result is a unit quaternion since all useful 
                ///quaternions need to be unit.
                quaternion operator + (const quaternion& rhs) const;

                ///Adds two quaternions together.
                ///
                ///This is a shortcut for "q = q + q2;". The resultant quaternion 
                ///will be a normalized quaternion since all useful quaternions have
                ///to be unit anyway.
                ///@see quaternion operator + (const quaternion& rhs) const;
                void operator += (const quaternion& rhs);

                ///Subtracts two quaternions from each other.
                ///@return The result is a unit quaternion since all useful 
                ///quaternions need to be unit.
                quaternion operator - (const quaternion& rhs) const;

                ///Subtracts two quaternions from each other.
                ///
                ///This is a shortcut for "q = q - q2;". The resultant quaternion 
                ///will be a normalized quaternion since all useful quaternions have 
                ///to be unit anyway.
                ///@see quaternion operator - (const quaternion& rhs) const;
                void operator -= (const quaternion& rhs);

                ///Divides two quaternions and returns the result.
                ///@return The result quaternion from the division of the two 
                ///quaternions.
                quaternion operator / (const quaternion& rhs) const;

                ///Divides two quaternions.
                ///
                ///Divides the quaternion on the left hand side by the one on the 
                ///right hand side. The left hand quaternion will obviously be 
                ///modified. This is a shortcut to "q = q / q2".
                void operator /= (const quaternion& rhs);

                ///Computes the dot product of two quaternions and returns the 
                ///result.
                ///
                ///This computes the dot product of two quaternions. This is exactly 
                ///the same as a vector dot product except that there is one extra 
                ///component involved. Note that quaternion multiplication is NOT a 
                ///dot product.
                ///@return The value of the dot product between two quaternions. 
                value_type dot(const quaternion& rhs);

                ///Returns the length of the quaternion
                ///
                ///Use this method to determine the length of a quaternion. The 
                ///algorithm is exactly the same as calculating a vectors length 
                ///(i.e. sqrt(x^2 + y^2 + z^2 + w^2).
                ///@return The lenth of the quaternion as a value_type.
                value_type length() const;

                ///Returns the squared length of the quaternion.
                ///
                ///This is the same as the normal length function except the costly 
                ///square root is not calculated. This is a useful function to 
                ///determine if there are differences in length.
                ///@see value_type length() const;
                ///@return The squared length of the quaternion.
                value_type length_squared() const;

                ///Negates the vector part of the quaternion and returns the result
                ///
                ///This method negates the quaternion. The result will be a 
                ///normalized negation of the quaternion
                ///@return A normalized and negated quaternion 
                quaternion negated() const;

                ///Negates the quaternion.
                ///
                ///This method negates the quaternion and normalizes it. If you 
                ///would just like a negated version of the quaternion without 
                ///modification then see negated().
                ///@see quaternion negated() const;
                void negate();

                ///Calculates the exponent of the quaternion and returns the result
                ///
                ///Use this method to get the exponent of the quaternion. The 
                ///algorithm is as follows:\n
                ///	1. Find the length of the quaternion.\n
                ///	2. Calculate the L = sin(length) / length.\n
                ///	3. Multiply the vector component of the quaternion by L.\n
                ///	4. Multiply the W component of the quaternion by cos(length).\n
                ///@return An exponent quaternion
                quaternion exponent() const;

                ///Returns the raw quaternion representation
                const value_type* raw_quaternion() const { return v;}
                value_type* raw_quaternion() {return v;}


                ///This is the SLERP algorithm.
                ///
                ///Smoothly ( spherically, shortest path on a quaternion 4D sphere) 
                ///iterpolates between two *UNIT* quaternion positions.
                ///@param to The source quaternion.
                ///@param from The destination quaternion.
                ///@param time The interpolation paramater between [0..1].
                ///@return the resulting inbetween quaternion.
                static quaternion slerp(const quaternion& from, const quaternion& to, value_type time);

                ///This is the LERP algorithm.
                ///
                ///Linearly interpolates between two quaternion positions and 
                ///returns the result. This is a much faster alrorithm than SLERP 
                ///but not as smooth.
                ///@param to The source quaternion.
                ///@param from The destination quaternion.
                ///@param time The interpolation paramater between [0..1].
                ///@return the resulting inbetween quaternion.
                static quaternion lerp(const quaternion& from, const quaternion& to, value_type time);

                ///Returns a rotation quaternion rotating from one vector direction 
                ///to another.
                ///
                ///Use this to create a quaternion which rotates from one vector 
                ///direction to another. Make sure the vectors are unit vectors!
                static quaternion rotate_to(const vector_3d<value_type>& from, const vector_3d<value_type>& to);


            private:
                value_type v[4]; ///< The quaternion representation. The order of 
                ///the values is x, y, z, and w.
        };

    typedef quaternion<float> quaternionf;
    typedef quaternion<double> quaterniond;


    //Implementation

    template <typename value_type>
        quaternion<value_type>::quaternion()
        {
            v[0] = v[1] = v[2] = 0;
            v[3] = 1;
        }

    template <typename value_type>			
        quaternion<value_type>::quaternion(value_type x, value_type y, value_type z, value_type w)
        {
            set(x, y, z, w);
        }

    template <typename value_type>
        void
        quaternion<value_type>::set(value_type x, value_type y, value_type z, value_type w)
        {
            v[0] = x;
            v[1] = y;
            v[2] = z;
            v[3] = w;
        }


    template <typename value_type>
        template <class other_value_type>
        quaternion<value_type>::quaternion(const quaternion<other_value_type>& quat)
        {
            std::copy(quat.raw_quaternion(), quat.raw_quaternion() + 4, v);
        }

    template <typename value_type>
        template <typename other_value_type>
        const quaternion<value_type>&
        quaternion<value_type>::operator = (const quaternion<other_value_type>& rhs)
        {
            std::copy(rhs.raw_quaternion(), rhs.raw_quaternion() + 4, v);
        }

    template <typename value_type>			
        template<class matrix_type>
        void 
        quaternion<value_type>::create_matrix(matrix_type& matrix)
        {
            value_type wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

            x2 =  v[0] + v[0]; 
            y2 =  v[1] + v[1]; 
            z2 =  v[2] + v[2];
            xx =  v[0] * x2;  
            xy =  v[0] * y2;   
            xz =  v[0] * z2;
            yy =  v[1] * y2;   
            yz =  v[1] * z2;   
            zz =  v[2] * z2;
            wx = v[3] * x2;   
            wy = v[3] * y2;   
            wz = v[3] * z2;

            matrix[0][0] = 1.0 - (yy + zz);
            matrix[0][1] = xy - wz;
            matrix[0][2] = xz + wy;
            matrix[0][3] = 0.0;

            matrix[1][0] = xy + wz;
            matrix[1][1] = 1.0 - (xx + zz);
            matrix[1][2] = yz - wx;
            matrix[1][3] = 0.0;

            matrix[2][0] = xz - wy;
            matrix[2][1] = yz + wx;
            matrix[2][2] = 1.0 - (xx + yy);
            matrix[2][3] = 0.0;

            matrix[3][0] = 0;
            matrix[3][1] = 0;
            matrix[3][2] = 0;
            matrix[3][3] = 1;
        }

    template <typename value_type>			
        void 
        quaternion<value_type>::create_from_euler(value_type roll, value_type pitch, value_type yaw)
        {
            value_type cr, cp, cy, sr, sp, sy, cpcy, spsy;
            cr = cos(roll/2.0);
            cp = cos(pitch/2.0);
            cy = cos(yaw/2.0);
            sr = sin(roll/2.0);
            sp = sin(pitch/2.0);
            sy = sin(yaw/2.0);

            cpcy = cp * cy;
            spsy = sp * sy;

            v[0] = sr * cpcy - cr * spsy;
            v[1]= cr * sp * cy + sr * cp * sy;
            v[2] = cr * cp * sy - sr * sp * cy;
            v[3] = cr * cpcy + sr * spsy;
        }

    template <typename value_type>		
        template <class matrix_type>
        void 
        quaternion<value_type>::create_from_matrix(const matrix_type& matrix)
        {
            value_type  tr, s;
            value_type  q[4];
            int i, j, k;
            int nxt[3] = {1, 2, 0};

            tr = matrix[0][0] + matrix[1][1] + matrix[2][2];

            // check the diagonal

            if (tr > 0.0) 
            {
                s = sqrt (tr + 1.0);

                v[3] = s / 2.0;

                s = 0.5 / s;

                v[0] = (matrix[1][2] - matrix[2][1]) * s;
                v[1] = (matrix[2][0] - matrix[0][2]) * s;
                v[2] = (matrix[0][1] - matrix[1][0]) * s;

            } else {		

                // diagonal is negative

                i = 0;

                if (matrix[1][1] > matrix[0][0]) i = 1;
                if (matrix[2][2] > matrix[i][i]) i = 2;

                j = nxt[i];
                k = nxt[j];

                s = sqrt ((matrix[i][i] - (matrix[j][j] + matrix[k][k])) + 1.0);

                q[i] = s * 0.5;

                if (s != 0.0) s = 0.5 / s;

                q[3] = (matrix[j][k] - matrix[k][j]) * s;
                q[j] =  (matrix[i][j]  + matrix[j][i]) * s;
                q[k] = (matrix[i][k] + matrix[k][i]) * s;

                v[0] = q[0];
                v[1] = q[1];
                v[2] = q[2];
                v[3] = q[3];
            }
        }

    template <typename value_type>		
        void 
        quaternion<value_type>::normalize()
        {
            value_type dist, square;
            square = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];

            if (square > 0.0)
                dist = static_cast<value_type>(1.0 / sqrt(square));
            else 
                dist = 1;

            v[0]  *= dist;
            v[1]  *= dist;
            v[2]  *= dist;
            v[3]  *= dist;
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::normalized() const
        {
            value_type dist, square;
            square = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];

            if (square > 0.0)
                dist = static_cast<value_type>(1.0 / sqrt(square));
            else 
                dist = 1;

            return quaternion( v[0]  * dist,
                    v[1]  * dist,
                    v[2]  * dist,
                    v[3]  * dist);

        }

    template <typename value_type>		
        vector_3d<value_type> 
        quaternion<value_type>::get_axis_angle(value_type& radians)
        {
            value_type	scale;
            vector_3d<value_type> vector(v[0], v[1], v[2]);

            scale = 1.0 / vector.length_squared();
            radians = static_cast<value_type>(2.0 * acos(v[3])); //where v[3] is w
            return vector * scale;
        }

    template <typename value_type>		
        void 
        quaternion<value_type>::create_from_axis_angle(vector_3d<value_type> axis, value_type radians)
        {
            axis.normalize();

            value_type result = static_cast<value_type>(sin( radians / 2.0 ));

            axis *= result;
            v[0] = axis.x();
            v[1] = axis.y();
            v[2] = axis.z();
            v[3] = static_cast<value_type>(cos(radians / 2.0));
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::get_inverse() const
        {
            //get a normalized version of the quaternion.
            quaternion<value_type>	inverse = normalized();

            //inverse the vector component of the quaternion
            inverse.v[0] = -inverse.v[0];
            inverse.v[1] = -inverse.v[1];
            inverse.v[2] = -inverse.v[2];
            return inverse;
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::operator * (const quaternion& rhs) const
        {
            quaternion temp( v[3] * rhs.v[0] + v[0] * rhs.v[3] + v[1] * rhs.v[2] - v[2] * rhs.v[1],
                    v[3] * rhs.v[1] + v[1] * rhs.v[3] + v[2] * rhs.v[0] - v[0] * rhs.v[2],
                    v[3] * rhs.v[2] + v[2] * rhs.v[3] + v[0] * rhs.v[1] - v[1] * rhs.v[0],
                    v[3] * rhs.v[3] - v[0] * rhs.v[0] - v[1] * rhs.v[1] - v[2] * rhs.v[2]);
            temp.normalize();
            return temp;
        }

    template <typename value_type>
        vector_3d<value_type>
        quaternion<value_type>::operator * (const vector_3d<value_type>& rhs) const
        {
            //barrowed from ogre3d source code
            vector_3d<value_type> uv, uuv;
            vector_3d<value_type> quat_vector(v[0], v[1], v[2]);
            uv = quat_vector.cross(rhs);
            uuv = quat_vector.cross(uv);
            uv *= (2.0f * v[3]);
            uuv *= 2.0f;

            return rhs + uv + uuv;
        };

    template <typename value_type>		
        void 
        quaternion<value_type>::operator *=(const quaternion& rhs)
        {
            *this = *this * rhs;
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::operator + (const quaternion& rhs) const
        {
            return quaternion( v[0] + rhs.v[0],
                    v[1] + rhs.v[1],
                    v[2] + rhs.v[2],
                    v[3] + rhs.v[3]).normalized();
        }

    template <typename value_type>		
        void 
        quaternion<value_type>::operator += (const quaternion& rhs)
        {
            v[0] += rhs.v[0];
            v[1] += rhs.v[1];
            v[2] += rhs.v[2];
            v[3] += rhs.v[3];
            normalize();
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::operator - (const quaternion& rhs) const
        {
            return quaternion( v[0] - rhs.v[0],
                    v[1] - rhs.v[1],
                    v[2] - rhs.v[2],
                    v[3] - rhs.v[3]).normalized();
        }

    template <typename value_type>		
        void 
        quaternion<value_type>::operator -= (const quaternion& rhs)
        {
            v[0] -= rhs.v[0];
            v[1] -= rhs.v[1];
            v[2] -= rhs.v[2];
            v[3] -= rhs.v[3];
            normalize();
        }

    template <typename value_type>
        quaternion<value_type> 
        quaternion<value_type>::operator / (const quaternion& rhs) const
        {
            quaternion temp = rhs;

            //invert vector component of the quaternion
            temp.negate();

            quaternion q = *this * temp;
            quaternion d = temp * temp;

            return quaternion( q.v[0] / d.v[3], //divide all components by w
                    q.v[1] / d.v[3],
                    q.v[2] / d.v[3],
                    q.v[3] / d.v[3]);
        }

    template <typename value_type>		
        void 
        quaternion<value_type>::operator /= (const quaternion& rhs)
        {
            quaternion temp = rhs;

            //invert vector component of the quaternion
            temp.negate();

            *this *= temp;
            quaternion d = temp * temp;

            v[0] /= d.v[3];
            v[1] /= d.v[3];
            v[2] /= d.v[3];
            v[3] /= d.v[3];
        }

    template <typename value_type>		
        value_type 
        quaternion<value_type>::dot(const quaternion& rhs)
        {
            return v[3] * rhs.v[3] + v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2];
        }

    template <typename value_type>		
        value_type 
        quaternion<value_type>::length() const
        {
            return sqrt(v[3] * v[3] + v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }

    template <typename value_type>		
        value_type 
        quaternion<value_type>::length_squared() const
        {
            return v[3] * v[3] + v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::negated() const
        {
            //negate the vector component
            return quaternion( -v[0],
                    -v[1],
                    -v[2],
                    v[3]);
        }

    template <typename value_type>		
        void 
        quaternion<value_type>::negate()
        {
            //invert the vector component of the quaternion
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
        }

    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::exponent() const
        {
            double  length1, length2;

            length1 = static_cast<value_type>(sqrt (v[0] * v[0] + v[1] *v[1] + v[2] *v[2]));
            if (length1 > 0.0) 
                length2 = static_cast<value_type>(sin(length1)) / length1; 
            else 
                length2 = 1.0;

            return quaternion( v[0] * length2,
                    v[1] * length2,
                    v[2] * length2,
                    cos(length1));
        }



    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::slerp(const quaternion& from, const quaternion& to, value_type time)
        {
            value_type        omega, sinom, cosom, scale0, scale1;

            value_type error = 0.001;
            // calculate cosine
            cosom = from.v[0] * to.v[0] + from.v[1] * to.v[1] + from.v[2] * to.v[2] + from.v[3] * to.v[3];

            // calculate coefficients
            // standard case (slerp)
            omega = acos(cosom);			
            sinom = sin(omega);

            if(sinom == 0.0)
                return from;

            scale0 = sin((1.0 - time)*omega) / sinom;
            scale1 = sin(time * omega) / sinom;

            scale1 =  cosom < 0.0? -scale1: scale1; //adjust signs if nessasary
            // calculate final quaternion
            return quaternion( scale0 * from.v[0] + scale1 * to.v[0],   //calculate x
                    scale0 * from.v[1] + scale1 * to.v[1],  //calculate y
                    scale0 * from.v[2] + scale1 * to.v[2],  //calculate z
                    scale0 * from.v[3] + scale1 * to.v[3]); //calculate w

        }	


    template <typename value_type>		
        quaternion<value_type> 
        quaternion<value_type>::lerp(const quaternion& from, const quaternion& to, value_type time)
        {
            value_type           to1[4];
            value_type          cosom;
            value_type          scale0, scale1;

            // calc cosine
            cosom = from.v[0] * to.v[0] + from.v[1] * to.v[1] + from.v[2] * to.v[2] + from.v[3] * to.v[3];

            // interpolate linearly
            scale0 = 1.0 - time;
            scale1 = cosom < 0.0 ? -time : time; //adjust the signs if nessasary

            // calculate final values
            return quaternion( scale0 * from.v[0] + scale1 * to1[0],   //calculate x
                    scale0 * from.v[1] + scale1 * to1[1],  //calculate y
                    scale0 * from.v[2] + scale1 * to1[2],  //calculate z
                    scale0 * from.v[3] + scale1 * to1[3]); //calculate w
        }

    template <typename value_type>
        quaternion<value_type> 
        quaternion<value_type>::rotate_to(const vector_3d<value_type>& from, const vector_3d<value_type>& to)
        {
            value_type w = to * from;
            vector_3d<value_type> component = to.cross(from);
            component.normalize();
            quaternion result(component.x(),
                    component.y(),
                    component.z(),
                    w);
            result.normalize();
            return result;

        }
} //nccentrifuge
#endif

