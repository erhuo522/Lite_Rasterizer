 

#include "agg_scanline.h"

#include <string.h>

namespace agg
{


	scanline::~scanline()
	{
		delete[] m_counts;
		delete[] m_start_ptrs;
		delete[] m_covers;
	}


	//------------------------------------------------------------------------
	scanline::scanline()
		: m_min_x(0),
		m_max_len(0),
		m_dx(0),
		m_dy(0),
		m_last_x(0x7FFF),
		m_last_y(0x7FFF),
		m_covers(0),
		m_start_ptrs(0),
		m_counts(0),
		m_num_spans(0),
		m_cur_start_ptr(0),
		m_cur_count(0)
	{
	}


	//------------------------------------------------------------------------
	//初始化扫描线
	void scanline::reset(int min_x, int max_x, int dx, int dy)
	{
		unsigned max_len = max_x - min_x + 2;
		if (max_len > m_max_len)
		{
			delete[] m_counts;
			delete[] m_start_ptrs;
			delete[] m_covers;
			m_covers = new unsigned char[max_len];
			m_start_ptrs = new unsigned char*[max_len];
			m_counts = new int16u[max_len];
			m_max_len = max_len;
		}
		m_dx = dx;
		m_dy = dy;
		m_last_x = 0x7FFF;
		m_last_y = 0x7FFF;
		m_min_x = min_x;
		m_cur_count = m_counts;
		m_cur_start_ptr = m_start_ptrs;
		m_num_spans = 0;
	}


	//------------------------------------------------------------------------
	//添加一个span，有num个像素
	void scanline::add_span(int x, int y, unsigned num, unsigned cover)
	{
		x -= m_min_x;

		//保存span中每一个像素的覆盖值
		memset(m_covers + x, cover, num);

		if (x == m_last_x + 1)
		{
			//添加span和上一个span相邻，则合并它们
			(*m_cur_count) += (int16u)num;
		}
		else
		{
			//保存span的数量
			*++m_cur_count = (int16u)num;
			//保存span的首地址
			*++m_cur_start_ptr = m_covers + x;
			//span数量加一
			m_num_spans++;
		}

		//记录最后一个点的坐标
		m_last_x = x + num - 1;
		m_last_y = y;
	}


	//------------------------------------------------------------------------
	//重置扫描线，已被下一条扫描线使用
	 void scanline::reset_spans()
	{
		m_last_x = 0x7FFF;
		m_last_y = 0x7FFF;
		m_cur_count = m_counts;
		m_cur_start_ptr = m_start_ptrs;
		m_num_spans = 0;
	}


	//------------------------------------------------------------------------
	 //添加一个span，只有一个像素
	 void scanline::add_cell(int x, int y, unsigned cover)
	{
		x -= m_min_x;
		m_covers[x] = (unsigned char)cover;
		if (x == m_last_x + 1)
		{
			(*m_cur_count)++;
		}
		else
		{
			*++m_cur_count = 1;
			*++m_cur_start_ptr = m_covers + x;
			m_num_spans++;
		}
		m_last_x = x;
		m_last_y = y;
	}


	//------------------------------------------------------------------------
	 int scanline::is_ready(int y) const
	{
		return m_num_spans && (y ^ m_last_y);
	}

}