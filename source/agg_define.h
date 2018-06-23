 
#ifndef AGG_DEFINE_INCLUDED
#define AGG_DEFINE_INCLUDED



namespace agg
{
    //------------------------------------------------------------------------
    typedef signed char    int8;
    typedef unsigned char  int8u;
    typedef signed short   int16;
    typedef unsigned short int16u;
    typedef signed int     int32;
    typedef unsigned int   int32u;


	enum filling_rule_e
	{
		fill_non_zero,
		fill_even_odd
	};

	enum
	{
		poly_base_shift = 8,
		poly_base_size = 1 << poly_base_shift,
		poly_base_mask = poly_base_size - 1
	};
  

	//像素单元
	struct cell
	{
		int16 x;              // x坐标
		int16 y;              // y坐标
		int   packed_coord;   // pack的xy坐标
		int   cover;          // 覆盖 
		int   area;           // 面积 

		void set(int x, int y, int c, int a);
		void set_coord(int x, int y);
		void set_cover(int c, int a);
		void add_cover(int c, int a);
	};
	 
}



#endif  //AGG_DEFINE_INCLUDED

