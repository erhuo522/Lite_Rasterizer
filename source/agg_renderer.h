 
#ifndef AGG_RENDERER_INCLUDED
#define AGG_RENDERER_INCLUDED

#include "agg_define.h" 
#include "agg_span.h"
#include "agg_scanline.h"
 
#include "rendering_buffer.h"


namespace agg
{
    
    //------------------------------------------------------------------------
    template<class Span> class renderer
    {
    public:
        //--------------------------------------------------------------------
        renderer(rendering_buffer& rbuf) : m_rbuf(&rbuf)
        {
        }
        
        //--------------------------------------------------------------------
		//���������ɫ
        void clear(const rgba8& c)
        {
            unsigned y;
            for(y = 0; y < m_rbuf->height(); y++)
            {
                m_span.hline(m_rbuf->row(y), 0, m_rbuf->width(), c);
            }
        }

        //--------------------------------------------------------------------
		//��Ⱦһ������
        void pixel(int x, int y, const rgba8& c)
        {
            if(m_rbuf->inbox(x, y))
            {
                m_span.hline(m_rbuf->row(y), x, 1, c);
            }
        }

        //--------------------------------------------------------------------
		//����һ������
        rgba8 pixel(int x, int y) const
        {
            if(m_rbuf->inbox(x, y))
            {
                return m_span.get(m_rbuf->row(y), x);
            }
            return rgba8(0,0,0);
        }

        //--------------------------------------------------------------------
		//��Ⱦɨ����
        void render(const scanline& sl, const rgba8& c)
        {
            if(sl.y() < 0 || sl.y() >= int(m_rbuf->height()))
            {
                return;
            }

            unsigned num_spans = sl.num_spans();
            int base_x = sl.base_x();
            unsigned char* row = m_rbuf->row(sl.y());
            scanline::iterator span(sl);


			//ѭ����Ⱦ����span
            do
            {
                int x = span.next() + base_x;
                const int8u* covers = span.covers();
                int num_pix = span.num_pix();

				//��span�淶����Ч��Χ
                if(x < 0)
                {
                    num_pix += x;
                    if(num_pix <= 0) continue;
                    covers -= x;
                    x = 0;
                } 
                if(x + num_pix >= int(m_rbuf->width()))
                {
                    num_pix = m_rbuf->width() - x;
                    if(num_pix <= 0) continue;
                }

				//��Ⱦspan
                m_span.render(row, x, num_pix, covers, c);
            }
            while(--num_spans);
        }

        //--------------------------------------------------------------------
        rendering_buffer& rbuf() { return *m_rbuf; }

    private:
        rendering_buffer* m_rbuf;
        Span              m_span;
    };

}



#endif //AGG_RENDERER_INCLUDED

