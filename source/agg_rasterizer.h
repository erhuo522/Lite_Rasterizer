 
#ifndef AGG_RASTERIZER_INCLUDED
#define AGG_RASTERIZER_INCLUDED

#include "agg_scanline.h"
#include "agg_outline.h"
#include "agg_rgba8.h"

#include <string.h>

namespace agg
{

	

	inline int poly_coord(double c)
	{
		return int(c * poly_base_size);
	}


	class rasterizer
	{
	public:
		enum
		{
			aa_shift = scanline::aa_shift,
			aa_num = 1 << aa_shift,
			aa_mask = aa_num - 1,
			aa_2num = aa_num * 2,
			aa_2mask = aa_2num - 1
		};

		rasterizer() :
			m_filling_rule(fill_non_zero)
		{
			memcpy(m_gamma, s_default_gamma, sizeof(m_gamma));
		}

		//--------------------------------------------------------------------
		void reset() { m_outline.reset(); }

		//--------------------------------------------------------------------
		void filling_rule(filling_rule_e filling_rule)
		{
			m_filling_rule = filling_rule;
		}

		//--------------------------------------------------------------------
		void move_to(int x, int y) { m_outline.move_to(x, y); }
		void line_to(int x, int y) { m_outline.line_to(x, y); }

		//--------------------------------------------------------------------
		void move_to_d(double x, double y) {
			m_outline.move_to(poly_coord(x),
				poly_coord(y));
		}
		void line_to_d(double x, double y) {
			m_outline.line_to(poly_coord(x),
				poly_coord(y));
		}

		//--------------------------------------------------------------------
		int min_x() const { return m_outline.min_x(); }
		int min_y() const { return m_outline.min_y(); }
		int max_x() const { return m_outline.max_x(); }
		int max_y() const { return m_outline.max_y(); }

		//--------------------------------------------------------------------
		unsigned calculate_alpha(int area) const
		{
			int cover = area >> (poly_base_shift * 2 + 1 - aa_shift);

			if (cover < 0) cover = -cover;
			if (m_filling_rule == fill_even_odd)
			{
				cover &= aa_2mask;
				if (cover > aa_num)
				{
					cover = aa_2num - cover;
				}
			}
			if (cover > aa_mask) cover = aa_mask;
			return cover;
		}

		//--------------------------------------------------------------------
		template<class Renderer> void render(Renderer& r,
			const rgba8& c,
			int dx = 0,
			int dy = 0)
		{
			const cell* const* cells = m_outline.cells();
			if (m_outline.num_cells() == 0) return;

			int x, y;
			int cover;
			int alpha;
			int area;

			m_scanline.reset(m_outline.min_x(), m_outline.max_x(), dx, dy);

			cover = 0;
			const cell* cur_cell = *cells++;
			for (;;)
			{
				const cell* start_cell = cur_cell;

				int coord = cur_cell->packed_coord;
				x = cur_cell->x;
				y = cur_cell->y;

				area = start_cell->area;
				cover += start_cell->cover;

				//accumulate all start cells
				while ((cur_cell = *cells++) != 0)
				{
					if (cur_cell->packed_coord != coord)
						break;
					area += cur_cell->area;
					cover += cur_cell->cover;
				}

				if (area)
				{
					alpha = calculate_alpha((cover << (poly_base_shift + 1)) - area);
					if (alpha)
					{
						if (m_scanline.is_ready(y))
						{
							r.render(m_scanline, c);
							m_scanline.reset_spans();
						}
						m_scanline.add_cell(x, y, m_gamma[alpha]);
					}
					x++;
				}

				if (!cur_cell) break;

				if (cur_cell->x > x)
				{
					alpha = calculate_alpha(cover << (poly_base_shift + 1));
					if (alpha)
					{
						if (m_scanline.is_ready(y))
						{
							r.render(m_scanline, c);
							m_scanline.reset_spans();
						}
						m_scanline.add_span(x, y,
							cur_cell->x - x,
							m_gamma[alpha]);
					}
				}
			}

			if (m_scanline.num_spans())
			{
				r.render(m_scanline, c);
			}
		}


		//--------------------------------------------------------------------
		bool hit_test(int tx, int ty);

	private:
		rasterizer(const rasterizer&);
		const rasterizer& operator = (const rasterizer&);

	private:
		outline        m_outline;
		scanline       m_scanline;
		filling_rule_e m_filling_rule;
		int8u          m_gamma[256];
		static const int8u s_default_gamma[256];
	};

}



#endif

