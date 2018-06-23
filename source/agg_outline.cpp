#include "agg_outline.h"

#include <string.h>

namespace agg
{
	//------------------------------------------------------------------------
	enum
	{
		not_closed = 1,
		sort_required = 2
	};

	//---------------------------------------------------------------------------

	enum
	{
		qsort_threshold = 9
	};


	//------------------------------------------------------------------------
	template <class T> static inline void swap_cells(T* a, T* b)
	{
		T temp = *a;
		*a = *b;
		*b = temp;
	}

	//------------------------------------------------------------------------
	template <class T> static inline bool less_than(T* a, T* b)
	{
		return (*a)->packed_coord < (*b)->packed_coord;
	}

	//------------------------------------------------------------------------
	outline::~outline()
	{
		delete[] m_sorted_cells;
		if (m_num_blocks)
		{
			cell** ptr = m_cells + m_num_blocks - 1;
			while (m_num_blocks--)
			{
				delete[] * ptr;
				ptr--;
			}
			delete[] m_cells;
		}
	}


	//------------------------------------------------------------------------
	outline::outline() :
		m_num_blocks(0),
		m_max_blocks(0),
		m_cur_block(0),
		m_num_cells(0),
		m_cells(0),
		m_cur_cell_ptr(0),
		m_sorted_cells(0),
		m_sorted_size(0),
		m_cur_x(0),
		m_cur_y(0),
		m_close_x(0),
		m_close_y(0),
		m_min_x(0x7FFFFFFF),
		m_min_y(0x7FFFFFFF),
		m_max_x(-0x7FFFFFFF),
		m_max_y(-0x7FFFFFFF),
		m_flags(sort_required)
	{
		m_cur_cell.set(0x7FFF, 0x7FFF, 0, 0);
	}


	//------------------------------------------------------------------------
	void outline::reset()
	{
		m_num_cells = 0;
		m_cur_block = 0;
		m_cur_cell.set(0x7FFF, 0x7FFF, 0, 0);
		m_flags |= sort_required;
		m_flags &= ~not_closed;
		m_min_x = 0x7FFFFFFF;
		m_min_y = 0x7FFFFFFF;
		m_max_x = -0x7FFFFFFF;
		m_max_y = -0x7FFFFFFF;
	}



	//------------------------------------------------------------------------
	void outline::allocate_block()
	{
		if (m_cur_block >= m_num_blocks)
		{
			if (m_num_blocks >= m_max_blocks)
			{
				cell** new_cells = new cell*[m_max_blocks + cell_block_pool];
				if (m_cells)
				{
					memcpy(new_cells, m_cells, m_max_blocks * sizeof(cell*));
					delete[] m_cells;
				}
				m_cells = new_cells;
				m_max_blocks += cell_block_pool;
			}
			m_cells[m_num_blocks++] = new cell[unsigned(cell_block_size)];
		}
		m_cur_cell_ptr = m_cells[m_cur_block++];
	}


	//------------------------------------------------------------------------
	void outline::add_cur_cell()
	{
		if (m_cur_cell.area | m_cur_cell.cover)
		{
			if ((m_num_cells & cell_block_mask) == 0)
			{
				if (m_num_blocks >= cell_block_limit) 
					return;
				allocate_block();
			}
			*m_cur_cell_ptr++ = m_cur_cell;
			m_num_cells++;
		}
	}



	//------------------------------------------------------------------------
	void outline::set_cur_cell(int x, int y)
	{
		if (m_cur_cell.packed_coord != (y << 16) + x)
		{
			add_cur_cell();
			m_cur_cell.set(x, y, 0, 0);
		}
	}



	//------------------------------------------------------------------------
	 void outline::render_scanline(int ey, int x1, int y1, int x2, int y2)
	{
		int ex1 = x1 >> poly_base_shift;
		int ex2 = x2 >> poly_base_shift;
		int fx1 = x1 & poly_base_mask;
		int fx2 = x2 & poly_base_mask;

		int delta, p, first, dx;
		int incr, lift, mod, rem;

		//trivial case. Happens often
		if (y1 == y2)
		{
			set_cur_cell(ex2, ey);
			return;
		}

		//everything is located in a single cell.  That is easy!
		if (ex1 == ex2)
		{
			delta = y2 - y1;
			m_cur_cell.add_cover(delta, (fx1 + fx2) * delta);
			return;
		}

		//ok, we'll have to render a run of adjacent cells on the same
		//scanline...
		p = (poly_base_size - fx1) * (y2 - y1);
		first = poly_base_size;
		incr = 1;

		dx = x2 - x1;

		if (dx < 0)
		{
			p = fx1 * (y2 - y1);
			first = 0;
			incr = -1;
			dx = -dx;
		}

		delta = p / dx;
		mod = p % dx;

		if (mod < 0)
		{
			delta--;
			mod += dx;
		}

		m_cur_cell.add_cover(delta, (fx1 + first) * delta);

		ex1 += incr;
		set_cur_cell(ex1, ey);
		y1 += delta;

		if (ex1 != ex2)
		{
			p = poly_base_size * (y2 - y1 + delta);
			lift = p / dx;
			rem = p % dx;

			if (rem < 0)
			{
				lift--;
				rem += dx;
			}

			mod -= dx;

			while (ex1 != ex2)
			{
				delta = lift;
				mod += rem;
				if (mod >= 0)
				{
					mod -= dx;
					delta++;
				}

				m_cur_cell.add_cover(delta, (poly_base_size)* delta);
				y1 += delta;
				ex1 += incr;
				set_cur_cell(ex1, ey);
			}
		}
		delta = y2 - y1;
		m_cur_cell.add_cover(delta, (fx2 + poly_base_size - first) * delta);
	}






	//------------------------------------------------------------------------
	void outline::render_line(int x1, int y1, int x2, int y2)
	{
		int ey1 = y1 >> poly_base_shift;
		int ey2 = y2 >> poly_base_shift;
		int fy1 = y1 & poly_base_mask;
		int fy2 = y2 & poly_base_mask;

		int dx, dy, x_from, x_to;
		int p, rem, mod, lift, delta, first, incr;

		if (ey1   < m_min_y) m_min_y = ey1;
		if (ey1 + 1 > m_max_y) m_max_y = ey1 + 1;
		if (ey2   < m_min_y) m_min_y = ey2;
		if (ey2 + 1 > m_max_y) m_max_y = ey2 + 1;

		dx = x2 - x1;
		dy = y2 - y1;

		//everything is on a single scanline
		if (ey1 == ey2)
		{
			render_scanline(ey1, x1, fy1, x2, fy2);
			return;
		}

		//Vertical line - we have to calculate start and end cells,
		//and then - the common values of the area and coverage for
		//all cells of the line. We know exactly there's only one 
		//cell, so, we don't have to call render_scanline().
		incr = 1;
		if (dx == 0)
		{
			int ex = x1 >> poly_base_shift;
			int two_fx = (x1 - (ex << poly_base_shift)) << 1;
			int area;

			first = poly_base_size;
			if (dy < 0)
			{
				first = 0;
				incr = -1;
			}

			x_from = x1;

			delta = first - fy1;
			m_cur_cell.add_cover(delta, two_fx * delta);

			ey1 += incr;
			set_cur_cell(ex, ey1);

			delta = first + first - poly_base_size;
			area = two_fx * delta;
			while (ey1 != ey2)
			{
				m_cur_cell.set_cover(delta, area);
				ey1 += incr;
				set_cur_cell(ex, ey1);
			}

			delta = fy2 - poly_base_size + first;
			m_cur_cell.add_cover(delta, two_fx * delta);
			return;
		}

		//ok, we have to render several scanlines
		p = (poly_base_size - fy1) * dx;
		first = poly_base_size;

		if (dy < 0)
		{
			p = fy1 * dx;
			first = 0;
			incr = -1;
			dy = -dy;
		}

		delta = p / dy;
		mod = p % dy;

		if (mod < 0)
		{
			delta--;
			mod += dy;
		}

		x_from = x1 + delta;
		render_scanline(ey1, x1, fy1, x_from, first);

		ey1 += incr;
		set_cur_cell(x_from >> poly_base_shift, ey1);

		if (ey1 != ey2)
		{
			p = poly_base_size * dx;
			lift = p / dy;
			rem = p % dy;

			if (rem < 0)
			{
				lift--;
				rem += dy;
			}
			mod -= dy;

			while (ey1 != ey2)
			{
				delta = lift;
				mod += rem;
				if (mod >= 0)
				{
					mod -= dy;
					delta++;
				}

				x_to = x_from + delta;
				render_scanline(ey1, x_from, poly_base_size - first, x_to, first);
				x_from = x_to;

				ey1 += incr;
				set_cur_cell(x_from >> poly_base_shift, ey1);
			}
		}
		render_scanline(ey1, x_from, poly_base_size - first, x2, fy2);
	}


	//------------------------------------------------------------------------
	void outline::move_to(int x, int y)
	{
		if ((m_flags & sort_required) == 0) 
		{
			reset();
		}
		
		if (m_flags & not_closed)
		{
			line_to(m_close_x, m_close_y);
		}

		set_cur_cell(x >> poly_base_shift, y >> poly_base_shift);

		m_close_x = m_cur_x = x;
		m_close_y = m_cur_y = y;
	}



	//------------------------------------------------------------------------
	void outline::line_to(int x, int y)
	{
		if ((m_flags & sort_required) && ((m_cur_x ^ x) | (m_cur_y ^ y)))
		{
			int c;
			//更新box值
			c = m_cur_x >> poly_base_shift;
			if (c < m_min_x) m_min_x = c;
			++c;
			if (c > m_max_x) m_max_x = c;

			c = x >> poly_base_shift;
			if (c < m_min_x) m_min_x = c;
			++c;
			if (c > m_max_x) m_max_x = c;

			render_line(m_cur_x, m_cur_y, x, y);

			//更新当前值
			m_cur_x = x;
			m_cur_y = y;

			m_flags |= not_closed;
		}
	}




	//------------------------------------------------------------------------
	void outline::qsort_cells(cell** start, unsigned num)
	{
		cell**  stack[80];
		cell*** top;
		cell**  limit;
		cell**  base;

		limit = start + num;
		base = start;
		top = stack;

		for (;;)
		{
			int len = int(limit - base);

			cell** i;
			cell** j;
			cell** pivot;

			if (len > qsort_threshold)
			{
				// we use base + len/2 as the pivot
				pivot = base + len / 2;
				swap_cells(base, pivot);

				i = base + 1;
				j = limit - 1;

				// now ensure that *i <= *base <= *j 
				if (less_than(j, i))
				{
					swap_cells(i, j);
				}

				if (less_than(base, i))
				{
					swap_cells(base, i);
				}

				if (less_than(j, base))
				{
					swap_cells(base, j);
				}

				for (;;)
				{
					do i++; while (less_than(i, base));
					do j--; while (less_than(base, j));

					if (i > j)
					{
						break;
					}

					swap_cells(i, j);
				}

				swap_cells(base, j);

				// now, push the largest sub-array
				if (j - base > limit - i)
				{
					top[0] = base;
					top[1] = j;
					base = i;
				}
				else
				{
					top[0] = i;
					top[1] = limit;
					limit = j;
				}
				top += 2;
			}
			else
			{
				// the sub-array is small, perform insertion sort
				j = base;
				i = j + 1;

				for (; i < limit; j = i, i++)
				{
					for (; less_than(j + 1, j); j--)
					{
						swap_cells(j + 1, j);
						if (j == base)
						{
							break;
						}
					}
				}
				if (top > stack)
				{
					top -= 2;
					base = top[0];
					limit = top[1];
				}
				else
				{
					break;
				}
			}
		}
	}





	//------------------------------------------------------------------------
	void outline::sort_cells()
	{
		if (m_num_cells == 0)
			return;
		if (m_num_cells > m_sorted_size)
		{
			delete[] m_sorted_cells;
			m_sorted_size = m_num_cells;
			m_sorted_cells = new cell*[m_num_cells + 1];
		}

		cell** sorted_ptr = m_sorted_cells;
		cell** block_ptr = m_cells;
		cell*  cell_ptr;

		unsigned nb = m_num_cells >> cell_block_shift;
		unsigned i;

		while (nb--)
		{
			cell_ptr = *block_ptr++;
			i = cell_block_size;
			while (i--)
			{
				*sorted_ptr++ = cell_ptr++;
			}
		}

		cell_ptr = *block_ptr++;
		i = m_num_cells & cell_block_mask;
		while (i--)
		{
			*sorted_ptr++ = cell_ptr++;
		}
		m_sorted_cells[m_num_cells] = 0;
		qsort_cells(m_sorted_cells, m_num_cells);
	}

	const cell* const* outline::cells()
	{
		if (m_flags & not_closed)
		{
			line_to(m_close_x, m_close_y);
			m_flags &= ~not_closed;
		}

		//Perform sort only the first time.
		if (m_flags & sort_required)
		{
			add_cur_cell();
			if (m_num_cells == 0) 
				return 0;
			
			sort_cells();
			m_flags &= ~sort_required;
		}
		return m_sorted_cells;
	}


}



