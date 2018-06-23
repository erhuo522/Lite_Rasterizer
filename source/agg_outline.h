 
#ifndef AGG_OUTLINE_INCLUDED
#define AGG_OUTLINE_INCLUDED

#include "agg_define.h"

namespace agg
{
	class outline
	{
		enum
		{
			cell_block_shift = 12,
			cell_block_size = 1 << cell_block_shift,
			cell_block_mask = cell_block_size - 1,
			cell_block_pool = 256,
			cell_block_limit = 1024
		};

	public:

		~outline();
		outline();

		void reset();

		void move_to(int x, int y);
		void line_to(int x, int y);

		int min_x() const { return m_min_x; }
		int min_y() const { return m_min_y; }
		int max_x() const { return m_max_x; }
		int max_y() const { return m_max_y; }

		unsigned num_cells() const { return m_num_cells; }
		const cell* const* cells();

	private:
		outline(const outline&);
		const outline& operator = (const outline&);

		//设置当前像素单元格坐标
		void set_cur_cell(int x, int y);
		void add_cur_cell();
		void sort_cells();
		void render_scanline(int ey, int x1, int y1, int x2, int y2);
		void render_line(int x1, int y1, int x2, int y2);
		void allocate_block();

		static void qsort_cells(cell** start, unsigned num);

	private:
		unsigned  m_max_blocks;   //单元格最大块数
		unsigned  m_num_blocks;   //单元格块数
		unsigned  m_cur_block;    //当前单元格块号

		unsigned  m_num_cells;    //像素单元格数量
		cell**    m_cells;        //像素单元格 
		cell*     m_cur_cell_ptr; //当前像素单元格指针

		cell**    m_sorted_cells;
		unsigned  m_sorted_size;
		cell      m_cur_cell;     //当前像素单元
		int       m_cur_x;
		int       m_cur_y;

		//闭合的起点
		int       m_close_x;
		int       m_close_y;

		//box值
		int       m_min_x;
		int       m_min_y;
		int       m_max_x;
		int       m_max_y;
		
		//flag
		unsigned  m_flags;
	};

	 
}



#endif  //AGG_DEFINE_INCLUDED

