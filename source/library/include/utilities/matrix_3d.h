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

#ifndef NCCENTRIFUGE_MATRIX3D_H
#define NCCENTRIFUGE_MAATRIX3D_H

#include <algorithm>
#include "utilities/vector_3d.h"

namespace ncc 
{	
    ///A Template 3D Matrix Class.
    ///
    ///This is a 3D matrix class used to store orientation and location of a 3D 
    ///object. The value_type template paramter is used to specify the precision 
    ///used by the matrix. value_type can be a foat, double, complex, or user 
    ///defined type. The degree_count template parameter is used to specify the 
    ///number of degrees held in the matrix's cosine and sine lookup tables. It 
    ///is recommended that the user uses the typedefs created in the header file 
    ///instead of the raw class. The typedefs are as follows
    ///		- matrix_3df   A floating point version of the matrix class with 
    ///		  degree_count set to 2048.
    ///		- matrix_3dd   A double floating point version of the matrix class 
    ///		  with degree_count set to 2048.
    ///		- matrix_3di   A fixed point math version of the matrix class with 
    ///		  degree_count set to 2048.
    ///
    ///A fixed point version is useful for systems that don't have a FPU.\n
    ///
    ///Remember the order of operations on a matrix are not communitive!
    template <typename value_type, unsigned int degree_count = 2049>
        class matrix_3d
        {
            public:
                static const unsigned int size = 16;

                ///Standard constructor initializes the matrix to the identity matrix.
                matrix_3d();

                ///Copy constructor.
                template<typename other_value_type, unsigned int other_degree_count>
                    matrix_3d(const matrix_3d<other_value_type, other_degree_count>& mat);

                ///Creates a matrix from a standard 4x4 C matrix
                ///
                ///Use this constructor to initialize a max_3d matrix with a 4x4 C 
                ///matrix.
                template<class c_matrix_value_type>
                    matrix_3d(const c_matrix_value_type c_matrix[16]);


                ///Assignment operator.
                template<typename other_value_type, unsigned int other_degree_count>
                    const matrix_3d& operator = (const matrix_3d<other_value_type, other_degree_count>& mat);

                ///Rotate the matrix using the sine and cosine lookup tables
                ///
                ///Rotate the matrix by x, y, and z angles. This method uses the matrix 
                ///sine and cosine lookup tables to do the calculations. The x, y, and z 
                ///angles are determined by 
                void fast_rotate(int discrete_x_angle, int discrete_y_angle, int discrete_z_angle);

                void rotate(value_type x_angle, value_type y_angle, value_type z_angle);

                ///Translates the matrix by x, y, and z deltas. 
                void translate(value_type x_translate, value_type y_translate, value_type z_translate);

                ///Scales the matrix by x, y, and z multiples.
                void scale(value_type x_scale, value_type y_scale, value_type z_scale);

                ///Shear the matrix by x, and y multiples along the z axis
                void z_shear(value_type x_shear, value_type y_shear);

                ///Add two vectors together and return the result as a matrix_3d.
                ///@return matrix_3d class as a result of "matrix1 + matrix2".
                matrix_3d operator + (const matrix_3d& rhs) const;

                ///Subtract two matrices together an return the result as a matrix_3d
                ///@return matrix_3d class as a result of "matrix1 - matrix2".
                matrix_3d operator - (const matrix_3d& rhs) const;

                ///Multiply two matrices together and return the result.
                ///@return matrix_3d class as a result of "matrix1 * matrix2."\n
                ///Remember matrix multiplication is not communitive.
                matrix_3d operator * (const matrix_3d& rhs) const;

                ///Adds the matrix on the right hand side to the current matrix.
                void operator += (const matrix_3d& rhs);

                ///Subtracts the matrix on the right hand side from the current matrix.
                void operator -= (const matrix_3d& rhs);

                ///Multiplies the matrix on the right hand side to the current matrix. 
                ///
                ///Multiplies the matrix on the right hand side to the current matrix. 
                ///This is an optimization to "matrix1 = matrix1 * martrix2". Remember 
                ///that matrix multiplication is not communitive.
                void operator *= (const matrix_3d& rhs);

                ///Returns a const pointer to the raw matrix data.
                const value_type* raw_matrix() const { return matrix;}

                ///Returns a pointer to the raw matrix data.
                value_type* raw_matrix() { return matrix;}


                ///This class is used to allow matrix_3d class to simulate the standard 
                ///C notation of a multidimensional array.
                ///
                ///This class is used by row_holder operator[](unsigned int row) to 
                ///simulate the C notation of a multidimensional
                ///array.\n
                ///An example:
                ///	- matrix_3df my_matrix;\n
                ///	  my_matrix[0][2];       //returns the value at row 0 and column 2\n
                ///
                ///Note that the matrix has no bounds checking.
                class row_holder
                {
                    public:
                        row_holder(value_type matrix[16], int row) : matrix_ptr(matrix), row_value(row){}
                        value_type& operator [](unsigned int column) {return matrix_ptr[column +row_value];} 
                        const value_type& operator [](unsigned int column) const { return matrix_ptr[column + row_value];}
                    private:
                        value_type* matrix_ptr;
                        unsigned int row_value;
                };

                ///Returns a reference to a value at a specific location in the matrix.
                ///
                ///Returns a reference to a value at a specific location in the matrix. 
                ///Use this to access the individual values in the matrix. Note that the
                ///matrix is a 4x4 matrix and that the first value always starts at 0. 
                ///This method does not provide overflow protection
                row_holder  operator[](unsigned int row);
                const row_holder operator[] (unsigned int row) const;

                ///Retrieves a value from the matrix by row and column.
                ///
                ///This method has no range checking so be careful. You must not access 
                ///any value outside of [0..3] for both row and column.
                ///@param row Specify the row in the range [0..3].
                ///@param column Specify the column in the range [0..3]
                ///@return The value at the row and column specified
                const value_type& operator()(unsigned int row, unsigned int column) const;
                ///@see const value_type& operator()(unsigned int row, unsigned int column) const;
                value_type&	operator()(unsigned int row, unsigned int column);

                ///Transforms a vector by the matrix class. 
                ///
                ///Use this class to transform vectors by the matrix class. 
                ///You can create a matrix with translation, scaling, and rotation 
                ///information which then can be used to transform the vector in 3d 
                ///space.
                template <typename vector_value_type>
                    vector_3d<vector_value_type> transform_vector(vector_3d<vector_value_type> vector) const;

                ///Turns the matrix into the identity matrix.
                ///
                ///Use this method to reset the matrix to be the identity matrix\n
                ///1 0 0 0\n
                ///0 1 0 0\n
                ///0 0 1 0\n
                ///0 0 0 1\n
                void make_identity();
            private:	


                ///Calculates the lookup table for the matrix classes.
                ///
                ///This method calculates the lookup table used by all matrix classes of 
                ///all types. This table is created once the first time a matrix class 
                ///is created. All matrices of the same type use the same table for the 
                ///fast_rotate method. the template parameter degree_count specifies the 
                ///number of degrees used in the table.
                ///\n\n
                ///The formula to calculate the closest descreet integer angle from and 
                ///angle between 0-2PI is
                ///		- discrete_angle = radian * (degree_count / 2PI)
                ///
                ///The reverse of this is obvious. 
                ///		- radian = discrete_angle * (2PI / degree_count)
                void calculate_lookup_table();

                static bool lookup_table_calculated;		///<Used to determine if the 
                ///lookup table was populated.

                static value_type sin_table[degree_count];	///<sine table used for quick 
                ///sine lookup at discrete 
                ///angles.

                static value_type cos_table[degree_count];	///<cosine table used for 
                ///quick cosine lookup at 
                ///discrete angles.

                ///Used internally to easly get the proper cosine value from the 
                ///discrete angle.
                value_type discrete_cos(int degree);

                ///Used internally to easly get the proper sine value from the discrete 
                ///angle.
                value_type discrete_sin(int degree);  	

                value_type matrix[16];

        };
#define NCC_PI 3.14159265358979323846264338327950288419716939937510f

    template <typename value_type, unsigned int degree_count>
        bool matrix_3d<value_type, degree_count>::lookup_table_calculated = false;

    template <typename value_type, unsigned int degree_count>
        value_type matrix_3d<value_type, degree_count>::sin_table[degree_count];

    template <typename value_type, unsigned int degree_count>
        value_type matrix_3d<value_type, degree_count>::cos_table[degree_count];

    ///A 3D matrix with floating point precision and 2048 discrete angles used 
    ///by fast_rotate.
    ///@see matrix_3d
    typedef matrix_3d<float, 2048> matrix_3df;

    ///A 3D matrix with double precision and 2048 discrete angles used by 
    ///fast_rotate.
    ///@see matrix_3d
    typedef matrix_3d<double, 2048> matrix_3dd;


    template <typename value_type, unsigned int degree_count>
        typename matrix_3d<value_type, degree_count>::row_holder
        matrix_3d<value_type, degree_count>::operator[](unsigned int row)
        {
            return row_holder(matrix, row*4);
        }

    template <typename value_type, unsigned int degree_count>
        const typename matrix_3d<value_type, degree_count>::row_holder
        matrix_3d<value_type, degree_count>::operator[](unsigned int row) const
        {
            return row_holder(matrix, row*4);
        }

    template <typename value_type, unsigned int degree_count>
        matrix_3d<value_type, degree_count>::matrix_3d()
        {
            calculate_lookup_table(); 
            make_identity();
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::make_identity()
        {
            matrix[0] = 1;	 matrix[1] = 0;	matrix[2] = 0;	matrix[3] = 0;
            matrix[4] = 0;	 matrix[5] = 1;	matrix[6] = 0;	matrix[7] = 0;
            matrix[8] = 0;	 matrix[9] = 0;	matrix[10] = 1;	matrix[11] = 0;
            matrix[12] = 0;	 matrix[13] = 0;	matrix[14] = 0;	matrix[15] = 1;
        }

    template <typename value_type, unsigned int degree_count>
        template<typename other_value_type, unsigned int other_degree_count>
        matrix_3d<value_type, degree_count>::matrix_3d(const matrix_3d<other_value_type, other_degree_count>& mat)
        {
            calculate_lookup_table(); 
            *this = mat;
        }

    template <typename value_type, unsigned int degree_count>
        template<typename other_value_type, unsigned int other_degree_count>
        const matrix_3d<value_type, degree_count>& 
        matrix_3d<value_type, degree_count>::operator = (const matrix_3d<other_value_type, other_degree_count>& mat)
        {
            std::copy(mat.raw_matrix(), mat.raw_matrix() + 16, matrix);
        }

    template <typename value_type, unsigned int degree_count>
        template<typename c_matrix_value_type>
        matrix_3d<value_type, degree_count>::matrix_3d(const c_matrix_value_type c_matrix[16])
        {
            std::copy(c_matrix, c_matrix + 16, matrix);
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::fast_rotate(int discrete_x_angle, int discrete_y_angle, int discrete_z_angle)
        {
            //This function uses the cosine and sin lookup tables at specified descrete angles. The amount of angles
            //around the circle are specifically specified by the "degree_count" template parameter.
            matrix_3d rotation_matrix;
            matrix_3d temp;
            //initialize Z axis rotation matrix - note: we perform Z
            //rotation first to align the 3d Z axis with the 2D Z axis
            temp.matrix[0] = discrete_cos(discrete_z_angle); temp.matrix[1] = discrete_sin(discrete_z_angle) ; temp.matrix[2] = 0; temp.matrix[3] = 0;
            temp.matrix[4] = -discrete_sin(discrete_z_angle); temp.matrix[5] = discrete_cos(discrete_z_angle); temp.matrix[6] = 0; temp.matrix[7] = 0;
            temp.matrix[8] = 0         ; temp.matrix[9] = 0         ;  temp.matrix[10] = 1; temp.matrix[11] = 0;
            temp.matrix[12] = 0         ; temp.matrix[13] = 0         ;  temp.matrix[14] = 0; temp.matrix[15] = 1;

            //merge temp with rotation matrix
            rotation_matrix *= temp;		

            //initialize X axis rotation matrix
            temp.matrix[0] = 1      ; temp.matrix[1] = 0         ; temp.matrix[2] = 0;          temp.matrix[3] = 0;
            temp.matrix[4] = 0      ; temp.matrix[5] = discrete_cos(discrete_x_angle); temp.matrix[6] =discrete_sin(discrete_x_angle);  temp.matrix[7] = 0;
            temp.matrix[8] = 0      ; temp.matrix[9] =-discrete_sin(discrete_x_angle) ; temp.matrix[10] =discrete_cos(discrete_x_angle); temp.matrix[11] = 0;
            temp.matrix[12] = 0      ; temp.matrix[13] = 0         ; temp.matrix[14] = 0;          temp.matrix[15] = 1;

            rotation_matrix *= temp;		

            //initialize Y axis rotation matrix
            temp.matrix[0] = discrete_cos(discrete_y_angle); temp.matrix[1] = 0      ; temp.matrix[2] =-discrete_sin(discrete_y_angle); temp.matrix[3] = 0;
            temp.matrix[4] = 0         ; temp.matrix[5] = 1      ; temp.matrix[6] = 0         ; temp.matrix[7] = 0;
            temp.matrix[8] = discrete_sin(discrete_y_angle); temp.matrix[9] = 0      ; temp.matrix[10] =discrete_cos(discrete_y_angle); temp.matrix[11] = 0;
            temp.matrix[12] = 0        ; temp.matrix[13] = 0      ; temp.matrix[14] = 0         ; temp.matrix[15] = 1;

            rotation_matrix *= temp;		


            //Merge rotation matrix with master matrix
            *this *= rotation_matrix;
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::rotate(value_type x_angle, value_type y_angle, value_type z_angle)
        {
            matrix_3d rotation_matrix;
            matrix_3d temp;
            //initialize Z axis rotation matrix - note: we perform Z
            //rotation first to align the 3d Z axis with the 2D Z axis
            temp.matrix[0] = cos(z_angle); temp.matrix[1] = sin(z_angle) ; temp.matrix[2] = 0; temp.matrix[3] = 0;
            temp.matrix[4] = -sin(z_angle); temp.matrix[5] = cos(z_angle); temp.matrix[6] = 0; temp.matrix[7] = 0;
            temp.matrix[8] = 0         ; temp.matrix[9] = 0         ;  temp.matrix[10] = 1; temp.matrix[11] = 0;
            temp.matrix[12] = 0         ; temp.matrix[13] = 0         ;  temp.matrix[14] = 0; temp.matrix[15] = 1;

            //merge temp with rotation matrix
            rotation_matrix *= temp;		

            //initialize X axis rotation matrix
            temp.matrix[0] = 1      ; temp.matrix[1] = 0         ; temp.matrix[2] = 0;          temp.matrix[3] = 0;
            temp.matrix[4] = 0      ; temp.matrix[5] = cos(x_angle); temp.matrix[6] =sin(x_angle);  temp.matrix[7] = 0;
            temp.matrix[8] = 0      ; temp.matrix[9] =-sin(x_angle) ; temp.matrix[10] =cos(x_angle); temp.matrix[11] = 0;
            temp.matrix[12] = 0      ; temp.matrix[13] = 0         ; temp.matrix[14] = 0;          temp.matrix[15] = 1;

            rotation_matrix *= temp;		

            //initialize Y axis rotation matrix
            temp.matrix[0] = cos(y_angle); temp.matrix[1] = 0      ; temp.matrix[2] =-sin(y_angle); temp.matrix[3] = 0;
            temp.matrix[4] = 0         ; temp.matrix[5] = 1      ; temp.matrix[6] = 0         ; temp.matrix[7] = 0;
            temp.matrix[8] = sin(y_angle); temp.matrix[9] = 0      ; temp.matrix[10] =cos(y_angle); temp.matrix[11] = 0;
            temp.matrix[12] = 0        ; temp.matrix[13] = 0      ; temp.matrix[14] = 0         ; temp.matrix[15] = 1;

            rotation_matrix *= temp;		


            //Merge rotation matrix with master matrix
            *this *= rotation_matrix;
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::translate(value_type x_translate, value_type y_translate, value_type z_translate)
        {
            //create 3d translation matrix
            matrix_3d translation;		

            //initialize translation matrix.
            //final translation matrix looks like:
            //1 0 0 0
            //0 1 0 0
            //0 0 1 0
            //x y z 1	
            translation.matrix[12] = x_translate; 
            translation.matrix[13] = y_translate; 
            translation.matrix[14] = z_translate; 

            //merge translation matrix with master matrix
            *this *= translation;
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::scale(value_type x_scale, value_type y_scale, value_type z_scale)
        {
            //create 3d scaling matrix
            matrix_3d scale;		

            //initialize scaling matrix
            //final scaling matrix looks like:
            //x 0 0 0
            //0 y 0 0
            //0 0 z 0
            //0 0 0 1
            scale.matrix[0] = x_scale;
            scale.matrix[5] = y_scale; 
            scale.matrix[10] = z_scale; 

            //merge scale matrix with master matrix
            *this *= scale;
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::z_shear(value_type x_shear, value_type y_shear)
        {
            //this method shears along the z axis
            //create 3d shearing matrix
            matrix_3d shear;	

            //initialize shearing matrix
            //final shearing matrix looks like:
            //1 0 x 0
            //0 1 y 0
            //0 0 1 0
            //0 0 0 1
            shear.matrix[2] = x_shear; 		
            shear.matrix[6] = y_shear;

            //merge mateix with master matrix
            *this = shear;
        }

    template <typename value_type, unsigned int degree_count>	
        matrix_3d<value_type, degree_count> 
        matrix_3d<value_type, degree_count>::operator + (const matrix_3d& rhs) const
        {
            matrix_3d temp;
            for(short row_col = 0; row_col < 16; ++row_col)
                temp.matrix[row_col] = matrix[row_col] + rhs.matrix[row_col];
            return temp;
        }

    template <typename value_type, unsigned int degree_count>
        matrix_3d<value_type, degree_count> 
        matrix_3d<value_type, degree_count>::operator - (const matrix_3d& rhs) const
        {
            matrix_3d temp;
            for(short row_col = 0; row_col < 16; ++row_col)
                temp.matrix[row_col] = matrix[row_col] - rhs.matrix[row_col];
            return temp;
        }

    template <typename value_type, unsigned int degree_count>
        matrix_3d<value_type, degree_count> 
        matrix_3d<value_type, degree_count>::operator * (const matrix_3d& rhs) const
        {
            matrix_3d temp;
            for (short i = 0; i < 16; i +=4)
                for (short j = 0; j <4; ++j)
                    temp.matrix[i+j] =  ( matrix[i +0] * rhs.matrix[0+j])
                        +(matrix[i+1] * rhs.matrix[4+j])
                        +(matrix[i+2] * rhs.matrix[8+j])
                        +(matrix[i+3] * rhs.matrix[12+j]);
            return temp;
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::operator += (const matrix_3d& rhs)
        {
            for(short row_col = 0; row_col < 16; ++row_col)
                matrix[row_col] += rhs.matrix[row_col];
        }

    template <typename value_type, unsigned int degree_count>
        void 
        matrix_3d<value_type, degree_count>::operator -= (const matrix_3d& rhs)
        {
            for(short row_col = 0; row_col < 16; ++row_col)
                matrix[row_col] -= rhs.matrix[row_col];
        }

    template <typename value_type, unsigned int degree_count>		
        void 
        matrix_3d<value_type, degree_count>::operator *= (const matrix_3d& rhs)
        {
            matrix_3d temp;
            for (short i = 0; i < 16; i +=4)
                for (short j = 0; j <4; ++j)
                    temp.matrix[i+j] =  ( matrix[i +0] * rhs.matrix[0+j])
                        +(matrix[i+1] * rhs.matrix[4+j])
                        +(matrix[i+2] * rhs.matrix[8+j])
                        +(matrix[i+3] * rhs.matrix[12+j]);
            *this = temp;
        }

    template <typename value_type, unsigned int degree_count>			
        template <typename vector_value_type>
        vector_3d<vector_value_type> 
        matrix_3d<value_type, degree_count>::transform_vector(vector_3d<vector_value_type> vector) const
        {
            return vector_3d<vector_value_type>(  (vector.x() * matrix[0])  //solve for x
                    +(vector.y() * matrix[4])
                    +(vector.z() * matrix[8])
                    +                  matrix[12],

                    (vector.x() * matrix[1])  //solve for y
                    +(vector.y() * matrix[5])
                    +(vector.z() * matrix[9])
                    +                  matrix[13],

                    (vector.x() * matrix[2])  //solve for z
                    +(vector.y() * matrix[6])
                    +(vector.z() * matrix[10])
                    +                  matrix[14]);
        }
    template <typename value_type, unsigned int degree_count>			
        void 
        matrix_3d<value_type, degree_count>::calculate_lookup_table()
        {
            if(!lookup_table_calculated) 
            {
                //this is used to convert the descrete angle to a floating point value
                value_type unit = static_cast<value_type>(NCC_PI * 2.0) / 
                    static_cast<value_type>(degree_count);
                //loop through all descrete degrees and populate the cosine and sine tables;
                for(unsigned int degree = 0; degree < degree_count; ++degree)
                {
                    value_type cast_degree = static_cast<value_type>(degree);
                    cos_table[degree] = cos(unit * cast_degree);
                    sin_table[degree] = sin(unit * cast_degree);
                }
                //set flag so that the table is only calculated once.
                lookup_table_calculated = true;
            }
        }

    template <typename value_type, unsigned int degree_count>			
        value_type 
        matrix_3d<value_type, degree_count>::discrete_cos(int degree)
        {
            return cos_table[degree & (degree_count - 1)];
        }

    template <typename value_type, unsigned int degree_count>			
        value_type 
        matrix_3d<value_type, degree_count>::matrix_3d<value_type, degree_count>::discrete_sin(int degree)
        {
            return sin_table[degree & (degree_count - 1)];
        }

    template <typename value_type, unsigned int degree_count>			
        const value_type& 
        matrix_3d<value_type, degree_count>::operator()(unsigned int row, unsigned int column) const
        {
            return matrix[(row*4)+ column];
        }

    template <typename value_type, unsigned int degree_count>			
        value_type& 
        matrix_3d<value_type, degree_count>::operator()(unsigned int row, unsigned int column)
        {
            return matrix[(row*4)+ column];
        }

} //nccentrifuge
#endif

