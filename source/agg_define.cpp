
#include "agg_define.h"


namespace agg
{

	//------------------------------------------------------------------------
	  void cell::set_cover(int c, int a)
	{
		cover = c;
		area = a;
	}

	//------------------------------------------------------------------------
	  void cell::add_cover(int c, int a)
	{
		cover += c;
		area += a;
	}

	//------------------------------------------------------------------------
	  void cell::set_coord(int cx, int cy)
	{
		x = int16(cx);
		y = int16(cy);
		packed_coord = (cy << 16) + cx;
	}

	//------------------------------------------------------------------------
	  void cell::set(int cx, int cy, int c, int a)
	{
		x = int16(cx);
		y = int16(cy);
		packed_coord = (cy << 16) + cx;
		cover = c;
		area = a;
	}


} 
