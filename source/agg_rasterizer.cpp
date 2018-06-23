 

#include "agg_rasterizer.h"


namespace agg
{
	//------------------------------------------------------------------------
	const int8u rasterizer::s_default_gamma[] =
	{
		0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8,
		9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18,
		19, 19, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28,
		29, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 37, 38,
		39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49,
		49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60,
		60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 71, 71,
		72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 83, 84,
		85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
		100, 101, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 114, 115,
		116, 117, 118, 119, 120, 121, 122, 123, 124, 126, 127, 128, 129, 130, 131, 132,
		134, 135, 136, 137, 139, 140, 141, 142, 144, 145, 146, 147, 149, 150, 151, 153,
		154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 174, 175,
		177, 178, 180, 181, 183, 184, 186, 188, 189, 191, 192, 194, 195, 197, 199, 200,
		202, 204, 205, 207, 209, 210, 212, 214, 215, 217, 219, 220, 222, 224, 225, 227,
		229, 230, 232, 234, 236, 237, 239, 241, 242, 244, 246, 248, 249, 251, 253, 255
	};


	//------------------------------------------------------------------------
	bool rasterizer::hit_test(int tx, int ty)
	{
		const cell* const* cells = m_outline.cells();
		if (m_outline.num_cells() == 0) return false;

		int x, y;
		int cover;
		int alpha;
		int area;

		cover = 0;
		const cell* cur_cell = *cells++;
		for (;;)
		{
			const cell* start_cell = cur_cell;

			int coord = cur_cell->packed_coord;
			x = cur_cell->x;
			y = cur_cell->y;

			if (y > ty) return false;

			area = start_cell->area;
			cover += start_cell->cover;

			while ((cur_cell = *cells++) != 0)
			{
				if (cur_cell->packed_coord != coord) break;
				area += cur_cell->area;
				cover += cur_cell->cover;
			}

			if (area)
			{
				alpha = calculate_alpha((cover << (poly_base_shift + 1)) - area);
				if (alpha)
				{
					if (tx == x && ty == y) return true;
				}
				x++;
			}

			if (!cur_cell) break;

			if (cur_cell->x > x)
			{
				alpha = calculate_alpha(cover << (poly_base_shift + 1));
				if (alpha)
				{
					if (ty == y && tx >= x && tx <= cur_cell->x) return true;
				}
			}
		}
		return false;
	}


}
 
