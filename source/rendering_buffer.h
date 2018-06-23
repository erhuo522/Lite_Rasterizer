  

#ifndef AGG_RENDERING_BUFFER_INCLUDED
#define AGG_RENDERING_BUFFER_INCLUDED

namespace agg
{


	class rendering_buffer
	{
	public:
		~rendering_buffer();

		//-----------------------------------------Initialization
		rendering_buffer(unsigned char* buf,
			unsigned width,
			unsigned height,
			int      stride);

		//-----------------------------------------Initialization
		void attach(unsigned char* buf,
			unsigned width,
			unsigned height,
			int      stride);

		//-----------------------------------------Acessors
		const unsigned char* buf()    const { return m_buf; }
		unsigned             width()  const { return m_width; }
		unsigned             height() const { return m_height; }
		int                  stride() const { return m_stride; }

		bool inbox(int x, int y) const
		{
			return x >= 0 && y >= 0 && x < int(m_width) && y < int(m_height);
		}

		unsigned abs_stride() const
		{
			return (m_stride < 0) ? unsigned(-m_stride) : unsigned(m_stride);
		}

		unsigned char* row(unsigned y) { return m_rows[y]; }
		const unsigned char* row(unsigned y) const { return m_rows[y]; }

	private:
		rendering_buffer(const rendering_buffer&);
		const rendering_buffer& operator = (const rendering_buffer&);

	private:
		unsigned char*  m_buf;        // Pointer to renrdering buffer
		unsigned char** m_rows;       // Pointers to each row of the buffer
		unsigned        m_width;      // Width in pixels
		unsigned        m_height;     // Height in pixels
		int             m_stride;     // Number of bytes per row. Can be < 0
		unsigned        m_max_height; // Maximal current height
	};

}

#endif// AGG_RENDERING_BUFFER_INCLUDED