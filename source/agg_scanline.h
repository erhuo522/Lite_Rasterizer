  

#ifndef AGG_SCANLINE_INCLUDED
#define AGG_SCANLINE_INCLUDED



#include "agg_define.h"



namespace agg
{


	class scanline
	{
	public:
		enum { aa_shift = 8 };

		class iterator
		{
		public:
			iterator(const scanline& sl) :
				m_covers(sl.m_covers),
				m_cur_count(sl.m_counts),
				m_cur_start_ptr(sl.m_start_ptrs)
			{
			}

			//下一个span
			int next()
			{
				++m_cur_count;
				++m_cur_start_ptr;
				return int(*m_cur_start_ptr - m_covers);
			}

			//span的像素数量
			int num_pix() const { return int(*m_cur_count); }
			//span中像素的覆盖值首地址
			const int8u* covers() const { return *m_cur_start_ptr; }

		private:
			const int8u*        m_covers;
			const int16u*       m_cur_count;
			const int8u* const* m_cur_start_ptr;
		};

		friend class iterator;

		~scanline();
		scanline();

		void     reset(int min_x, int max_x, int dx = 0, int dy = 0);

		void     reset_spans();
		void     add_cell(int x, int y, unsigned cover);
		void     add_span(int x, int y, unsigned len, unsigned cover);
	
		//检查扫描线是否不为空
		int      is_ready(int y) const;

		//最小的x坐标
		int      base_x()    const { return m_min_x + m_dx; }

		//扫描线的y坐标
		int      y()         const { return m_last_y + m_dy; }

		//扫描线中span的数量
		unsigned num_spans() const { return m_num_spans; }

	private:
		scanline(const scanline&);
		const scanline& operator = (const scanline&);

	private:
		int      m_min_x;         //扫描线中最小x值 
		unsigned m_max_len;       //扫描线最大长度

		//好像无用
		int      m_dx;           
		int      m_dy;
		
		//最后一个点的坐标
		int      m_last_x;
		int      m_last_y;

		int8u*   m_covers;       //扫描线中所有像素的覆盖值 
		int8u**  m_start_ptrs;   //所有span的首地址
		int16u*  m_counts;       //每个span中的像素数量
		unsigned m_num_spans;    //扫描线中span的数量
		
		int8u**  m_cur_start_ptr; //当前span首地址指针
		int16u*  m_cur_count;     //当前span数量指针
	};




}

#endif// AGG_SCANLINE_INCLUDED