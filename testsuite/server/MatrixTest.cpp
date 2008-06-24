// 
//   Copyright (C) 2005, 2006, 2007, 2008 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifdef HAVE_CONFIG_H
#include "gnashconfig.h"
#endif

#include "matrix.h"
#include <iostream>
#include <sstream>
#include <cassert>

#include "check.h"

using namespace std;
using namespace gnash;

// for double comparison
struct D {
	double _d;
	double _t; // tolerance

	D(double d) : _d(d), _t(1e-3) {}

	// Set tolerance
	D(double d, double t) : _d(d), _t(t) {}

	// Return true if the difference between the two
	// doubles is below the minimum tolerance defined for the two
	bool operator==(const D& d)
	{
		double tol = std::min(_t, d._t);
		double delta = fabs(_d - d._d);
		bool ret = delta < tol;
		//cout << "D " << _d << "operator==(const D " << d._d <<") returning " << ret << " (delta is " << delta << ") " << endl;
		return ret;
	}
};
std::ostream& operator<<(std::ostream& os, const D& d)
{
	return os << d._d << " [tol: " << d._t << "]";
}

int
main(int /*argc*/, char** /*argv*/)
{
    std::string label;

	// 
    //  Test identity matrix.
    // 
	matrix identity; 
	check(identity.is_valid());
	check_equals(identity.get_x_scale(), 1);
	check_equals(identity.get_y_scale(), 1);
	check_equals(identity.get_rotation(), 0);
	check_equals(identity.get_x_translation(), 0);
	check_equals(identity.get_y_translation(), 0);

	check_equals(identity.invert(), identity);

	//
	// Test parameter setting and getting, interfaces for AS.
	//
	matrix m1;
	m1.set_scale_rotation(1, 3, 0);
	check_equals(m1.get_x_scale(), 1);
	check_equals(m1.get_y_scale(), 3);
	check_equals(m1.get_rotation(), 0);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

    m1.set_scale(1.5, 2.5);
	check_equals(D(m1.get_x_scale()), 1.5);
	check_equals(D(m1.get_y_scale()), 2.5);
	check_equals(D(m1.get_rotation()), 0);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

	m1.set_scale(34, 4);
	check_equals(D(m1.get_x_scale()), 34);
	check_equals(D(m1.get_y_scale()), 4);
	check_equals(D(m1.get_rotation()), 0);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

	m1.set_scale_rotation(1, 1, 2);
	check_equals(D(m1.get_x_scale()), 1);
	check_equals(D(m1.get_y_scale()), 1);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

	m1.set_x_scale(2);
	check_equals(D(m1.get_x_scale()), 2);
	check_equals(D(m1.get_y_scale()), 1);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

	m1.set_scale(1, 2);
	check_equals(D(m1.get_x_scale()), 1);
	check_equals(D(m1.get_y_scale()), 2);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

	m1.set_rotation(0);
	check_equals(D(m1.get_x_scale()), 1);
	check_equals(D(m1.get_y_scale()), 2);
	check_equals(D(m1.get_rotation()), 0);
	check_equals(m1.get_x_translation(), 0);
	check_equals(m1.get_y_translation(), 0);

	m1.set_translation(5, 6);
	check_equals(D(m1.get_x_scale()), 1);
	check_equals(D(m1.get_y_scale()), 2);
	check_equals(D(m1.get_rotation()), 0);
	check_equals(m1.get_x_translation(), 5);
	check_equals(m1.get_y_translation(), 6);

	m1.set_rotation(2);
	check_equals(D(m1.get_x_scale()), 1);
	check_equals(D(m1.get_y_scale()), 2);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 5);
	check_equals(m1.get_y_translation(), 6);

	//
	// Test matrix concatenation
	//
	m1.concatenate_scale(2, 2);
	check_equals(D(m1.get_x_scale()), 2);
	check_equals(D(m1.get_y_scale()), 4);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 5);
	check_equals(m1.get_y_translation(), 6);

	m1.concatenate_scale(3, 3);
	check_equals(D(m1.get_x_scale()), 6);
	check_equals(D(m1.get_y_scale()), 12);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 5);
	check_equals(m1.get_y_translation(), 6);

	m1.concatenate_scale(2, 1);
	check_equals(D(m1.get_x_scale()), 12);
	check_equals(D(m1.get_y_scale()), 12);
	check_equals(D(m1.get_rotation()), 2);
	check_equals(m1.get_x_translation(), 5);
	check_equals(m1.get_y_translation(), 6);

	//
	// Test matrix transformations
	//
	point p1(0, 0);
	point p2(64, 64);
	point r;

	m1.set_identity();
	// Make a distance of 64 become a distance of 20 .. 
	m1.set_scale(20.0/64, 20.0/64);

	m1.transform(&r, p1);
	check_equals(r.x, 0);
	check_equals(r.y, 0);
   
	m1.transform(&r, p2);
	check_equals(r.x, 20);
	check_equals(r.y, 20);

	// Translate points to have the origin at 32,32
	// (coordinates expressed in prior-to-scaling matrix)
	m1.concatenate_translation(-32, -32);

	m1.transform(&r, p1);
	check_equals(r.x, -10);
	check_equals(r.y, -10);

	m1.transform(&r, p2);
	check_equals(r.x, 10);
	check_equals(r.y, 10);

	//  Apply a final scaling by 10 keeping the current origin 
    // (reached after translation)
	matrix final;
	final.set_scale(10, 10);
	final.concatenate(m1);
	m1 = final;

	m1.transform(&r, p1);
	check_equals(r.x, -100);
	check_equals(r.y, -100);

	m1.transform(&r, p2);
	check_equals(r.x, 100);
	check_equals(r.y, 100);

    //
	// Test matrix invertion
    // 
    m1.set_identity();
    m1.set_translation(50*20, -30*20);
    m1.set_scale(0.5, 2);
    m1.set_rotation(90*3.141593/180);
    
    matrix m1_inverse = m1;
    m1_inverse.invert();
    // concatenate the inverse matrix and orignial matrix.
    m1_inverse.concatenate(m1); 
    // the result is expected to be an identity matrix. 
    check_equals(m1_inverse, identity);

    m1.sx  = 4;   // 1/16384
    m1.shy = 0;
    m1.tx  = 20;
    m1.shx = 0;
    m1.sy  = 4;   // 1/16384
    m1.ty  = 20;
    
    m1_inverse = m1;
    m1_inverse.invert();
    
    check_equals(m1_inverse.sx, 16384 * 65536);
    check_equals(m1_inverse.shy, 0);
    check_equals(m1_inverse.tx, -16384 * 20);
    check_equals(m1_inverse.shx, 0);
    check_equals(m1_inverse.sy, 16384 * 65536);
    check_equals(m1_inverse.ty, -16384 * 20);
    
    // concatenate the inverse matrix and orignial matrix.
    m1_inverse.concatenate(m1);
    // the result is expected to be an identity matrix. 
    check_equals(m1_inverse, identity);
}

