 

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
	//��ʼ��ɨ����
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
	//���һ��span����num������
	void scanline::add_span(int x, int y, unsigned num, unsigned cover)
	{
		x -= m_min_x;

		//����span��ÿһ�����صĸ���ֵ
		memset(m_covers + x, cover, num);

		if (x == m_last_x + 1)
		{
			//���span����һ��span���ڣ���ϲ�����
			(*m_cur_count) += (int16u)num;
		}
		else
		{
			//����span������
			*++m_cur_count = (int16u)num;
			//����span���׵�ַ
			*++m_cur_start_ptr = m_covers + x;
			//span������һ
			m_num_spans++;
		}

		//��¼���һ���������
		m_last_x = x + num - 1;
		m_last_y = y;
	}


	//------------------------------------------------------------------------
	//����ɨ���ߣ��ѱ���һ��ɨ����ʹ��
	 void scanline::reset_spans()
	{
		m_last_x = 0x7FFF;
		m_last_y = 0x7FFF;
		m_cur_count = m_counts;
		m_cur_start_ptr = m_start_ptrs;
		m_num_spans = 0;
	}


	//------------------------------------------------------------------------
	 //���һ��span��ֻ��һ������
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