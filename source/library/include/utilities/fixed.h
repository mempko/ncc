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

#ifndef NCCENTRIFUGE_FIXED_H
#define NCCENTRIFUGE_FIXED_H
#include "utilities/pow.h"
namespace ncc 
{
    ///A Templated fixed point number class
    template<typename storage, int point>
        class fixed 
        {
            public:
                static const unsigned int float_resolution = pow<2,point>::value;
                
                fixed() : value(0){}
                fixed(storage val, bool junk) : value(val){}
                template<typename st, int pt>
                    fixed(const fixed<st,pt>& other) : 
                        fixed(static_cast<double>(other)){} 
                fixed(long val) {value = val << point;}
                fixed(int val) {value = val << point;
                fixed(short val)  {value = val << point;}
                fixed(double val) {value = val * float_resolution;} 
                fixed(float val) {value = val * float_resolution;}

                operator float() {return (float)value / float_resolution;}
                operator double() { return (double)value / float_resolution;}
                operator unsigned short() {return value >> point;}
                operator long() {return value >> point;}
                operator short() {return value >> point;}
                operator int() { return value >> point;}

                fixed operator * (const fixed& other) const
                { 
                    return fixed((value * other.value) >> point ,true);
                } 
                
                fixed operator / (const fixed& other) const 
                { 
                    return fixed((value << point) / other.value,true);
                }
                
                const fixed& operator *= (const fixed& other) 
                { 
                    value = (value * other.value) >> point; return *this;
                } 
                
                const fixed& operator /= (const fixed& other) 
                { 
                    value = (value << point) / other.value; return *this;
                }

                fixed operator + (const fixed& other) const { return fixed(value + other.value, true);} 
                fixed operator - (const fixed& other) const { return fixed(value - other.value, true);}
                const fixed& operator += (const fixed& other) { value += other.value; return *this;} 
                const fixed& operator -= (const fixed& other) { value -= other.value; return *this;}
                const fixed& operator =(const fixed& other) { value = other.value; return *this;}
                fixed operator % (const fixed& other) const { return fixed(value % other.value, true);} 
                bool operator == (const fixed& other) const {return value == other.value;} 
                bool operator != (const fixed& other) const {return value != other.value;} 
                bool operator < (const fixed& other) const {return value < other.value;}
                bool operator <= (const fixed& other) const {return value <= other.value;}
                bool operator > (const fixed& other) const {return value > other.value;}
                bool operator >= (const fixed& other) const {return value >= other.value;}
                template<>
                    friend fixed sqrt(fixed value);
                template<> 
                    friend fixed pow(fixed value); 
            private:
                storage value; 
        };

//////Some helpful typedefs
        typedef fixed<long, 8> fixed_long8;
        typedef fixed<long, 16> fixed_long16;
//////Implemination Methods
        template<typename storage, int point>
            fixed<storage,point> sqrt(fixed<storagte, point> value) 
            {
                typedef fixed<storage, point> fixed_type;
                if(value <= 0)
                    return false;
                fixed_type result = value / static_cast<fixed_type>(2);
                fixed_type error(1.0 / value.float_resolution, true);  
                fixed_type delta;
                do 
                {
                    delta = (result * result) - value;
                    result -= delta / (res * static_cast<fixed_type>(2));
                } 
                while ( delta > error || delta < -error);
                return result;
            }        

        template<typename storage, int point>
            fixed<storage,point> operator-(const fixed<storage,point>& value)
            {
                return static_cast<fixed_type<storage,point> >(0) - value;
            } 
} //nccentrifuge
#endif
