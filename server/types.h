// types.h	-- Thatcher Ulrich <tu@tulrich.com> 2003

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Some basic types.

#ifndef __TYPES_H__
#define __TYPES_H__


#include "gnash.h"
#include "tu_types.h"


namespace gnash {
	extern bool	s_verbose_action;
	extern bool	s_verbose_parse;
	extern bool	s_verbose_debug;
}


#define TWIPS_TO_PIXELS(x)	((x) / 20.f)
#define PIXELS_TO_TWIPS(x)	((x) * 20.f)


namespace gnash {
	struct stream;	// forward declaration

	/// RGBA record
	class rgba
	{
	public:
		uint8_t	m_r, m_g, m_b, m_a;

		/// Default RGBA value is FF.FF.FF.FF
		rgba() : m_r(255), m_g(255), m_b(255), m_a(255) {}

		/// Construct an RGBA with the provided values
		//
		/// @param r Red
		/// @param g Green
		/// @param b Blue
		/// @param a Alpha (transparency)
		///
		rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			:
			m_r(r), m_g(g), m_b(b), m_a(a)
		{
		}

		/// Initialize from intput stream.
		//
		///
		/// @param in
		///	The input (SWF) stream
		///
		/// @param tag_type 
		///	I don't know by which logic but
		///	a value <= 22 makes it read RGB
		///	and value > 22 makes it read RGBA
		///
		void	read(stream* in, int tag_type);

		/// Initialize from intput stream (reads RGBA)
		void	read_rgba(stream* in);

		/// Initialize from intput stream (reads RGB)
		void	read_rgb(stream* in);

		/// Set r,g,b.a values
		void	set(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		{
			m_r = r;
			m_g = g;
			m_b = b;
			m_a = a;
		}

		void	set_lerp(const rgba& a, const rgba& b, float f);

		/// Debug log.
		void	print();
	};


}	// end namespace gnash


#endif // __TYPES_H__


// Local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
