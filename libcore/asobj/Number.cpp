// Number.cpp:  ActionScript Number class, for Gnash.
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
//


#include "log.h"
#include "Number.h"
#include "smart_ptr.h"
#include "fn_call.h"
#include "as_object.h" // for inheritance
#include "as_value.h" // for doubleToString
#include "builtin_function.h" // need builtin_function
#include "VM.h" // for registering static GcResources (constructor and prototype)
#include "Object.h" // for getObjectInterface

#include <sstream>
#include <cmath>

namespace gnash {

// Forward declarations
//static void number_val_to_str(double val, char *str);
//static as_value number_to_string(const fn_call& fn);
static void attachNumberInterface(as_object& o);

static as_object*
getNumberInterface()
{
	static boost::intrusive_ptr<as_object> o=NULL;
	if ( o == NULL )
	{
		o = new as_object(getObjectInterface());
		VM::get().addStatic(o.get());

		attachNumberInterface(*o);
	}
	return o.get();
}

// FIXME: add some useful methods :)
class number_as_object : public as_object
{
	// the number value
	double _val;

	// string representation of the above number
	//mutable char _str[256];

public:

	number_as_object(double val=0.0)
		:
		as_object(getNumberInterface()),
		_val(val)
	{
	}

	// override from as_object
	std::string get_text_value() const
	{
		return as_value::doubleToString(_val); // number_val_to_str(_val, _str);
		//return _str;
	}

	// override from as_object
	double get_numeric_value() const
	{
		return _val;
	}

	as_value get_primitive_value() const 
	{
		return _val;
	}

	static as_value toString_method(const fn_call& fn);

	static as_value valueOf_method(const fn_call& fn);
};

static void
attachNumberInterface(as_object& o)
{
	// FIXME: add Number interface here:
	// Number.MAX_VALUE
	// Number.MIN_VALUE
	// Number.NaN
	// Number.NEGATIVE_INFINITY
	// Number.POSITIVE_INFINITY

	// Number.toString()
	o.init_member("toString", new builtin_function(number_as_object::toString_method));

	// Number.valueOf()
	o.init_member("valueOf", new builtin_function(number_as_object::valueOf_method));
}


static as_value
number_ctor(const fn_call& fn)
{
	double val = 0;
	if (fn.nargs > 0)
	{
		val = fn.arg(0).to_number();
	}

	if ( ! fn.isInstantiation() )
	{
		return as_value(val);
	}

	number_as_object* obj = new number_as_object(val);
	
	return as_value(obj); // will keep alive
}

static boost::intrusive_ptr<builtin_function> 
getNumberConstructor()
{
	// This is going to be the global Number "class"/"function"
	static boost::intrusive_ptr<builtin_function> cl=NULL;

	if ( cl == NULL )
	{
		cl=new builtin_function(&number_ctor, getNumberInterface());
		VM::get().addStatic(cl.get());

		// constants flags
		int cflags = as_prop_flags::dontEnum|as_prop_flags::dontDelete|as_prop_flags::readOnly;

		// change __proto__ constructor and prototype to have constant flags..
		as_value nullVal; nullVal.set_null();
		cl->setPropFlags(nullVal, 0, cflags);

		cl->init_member("MAX_VALUE", std::numeric_limits<double>::max(), cflags);
		cl->init_member("MIN_VALUE", std::numeric_limits<double>::denorm_min(), cflags);
		cl->init_member("NaN", as_value(NaN), cflags);
		cl->init_member("POSITIVE_INFINITY", as_value(std::numeric_limits<double>::infinity()), cflags);
		cl->init_member("NEGATIVE_INFINITY", as_value(-std::numeric_limits<double>::infinity()), cflags);

	}

	return cl;
}

// extern (used by Global.cpp)
void number_class_init(as_object& global)
{
	boost::intrusive_ptr<builtin_function> cl=getNumberConstructor();

	// Register _global.Number
	global.init_member("Number", cl.get());

}

boost::intrusive_ptr<as_object>
init_number_instance(double val)
{
	boost::intrusive_ptr<builtin_function> cl=getNumberConstructor();

	as_environment env(VM::get());

	std::auto_ptr< std::vector<as_value> > args ( new std::vector<as_value> );
	args->push_back(val);

	return cl->constructInstance(env, args);
}

as_value
number_as_object::toString_method(const fn_call& fn)
{
	// Number.toString must only work for number object, not generic ones.
	// This is so trace(Number.prototype) doesn't return 0 ...
	boost::intrusive_ptr<number_as_object> obj = ensureType<number_as_object>(fn.this_ptr);

	double val = obj->get_numeric_value();
	unsigned radix=10;

	if ( fn.nargs ) 
	{
		int userRadix = fn.arg(0).to_int();
		if ( userRadix >= 2 && userRadix <= 36 ) radix=userRadix;
		else
		{
			IF_VERBOSE_ASCODING_ERRORS(
			log_aserror(_("Number.toString(%s): "
				"radix must be in the 2..36 range (%d is invalid)"),
				fn.arg(0), userRadix)
			)
		}

	}
	return as_value::doubleToString(val, radix); 
}

as_value
number_as_object::valueOf_method(const fn_call& fn)
{
	// Number.valueOf must only work for number object, not generic ones.
	// This is so trace(Number.prototype == Object) return true in swf5 ?
	boost::intrusive_ptr<number_as_object> obj = ensureType<number_as_object>(fn.this_ptr);

	return obj->get_primitive_value();
}
  
} // namespace gnash