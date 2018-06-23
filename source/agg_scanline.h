  

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

			//��һ��span
			int next()
			{
				++m_cur_count;
				++m_cur_start_ptr;
				return int(*m_cur_start_ptr - m_covers);
			}

			//span����������
			int num_pix() const { return int(*m_cur_count); }
			//span�����صĸ���ֵ�׵�ַ
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
	
		//���ɨ�����Ƿ�Ϊ��
		int      is_ready(int y) const;

		//��С��x����
		int      base_x()    const { return m_min_x + m_dx; }

		//ɨ���ߵ�y����
		int      y()         const { return m_last_y + m_dy; }

		//ɨ������span������
		unsigned num_spans() const { return m_num_spans; }

	private:
		scanline(const scanline&);
		const scanline& operator = (const scanline&);

	private:
		int      m_min_x;         //ɨ��������Сxֵ 
		unsigned m_max_len;       //ɨ������󳤶�

		//��������
		int      m_dx;           
		int      m_dy;
		
		//���һ���������
		int      m_last_x;
		int      m_last_y;

		int8u*   m_covers;       //ɨ�������������صĸ���ֵ 
		int8u**  m_start_ptrs;   //����span���׵�ַ
		int16u*  m_counts;       //ÿ��span�е���������
		unsigned m_num_spans;    //ɨ������span������
		
		int8u**  m_cur_start_ptr; //��ǰspan�׵�ַָ��
		int16u*  m_cur_count;     //��ǰspan����ָ��
	};




}

#endif// AGG_SCANLINE_INCLUDED