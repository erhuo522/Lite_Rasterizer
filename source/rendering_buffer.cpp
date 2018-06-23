
#include  "rendering_buffer.h"

namespace agg
{

	//------------------------------------------------------------------------
	rendering_buffer::~rendering_buffer()
	{
		delete[] m_rows;
	}


	//------------------------------------------------------------------------
	rendering_buffer::rendering_buffer(unsigned char* buf,
		unsigned width,
		unsigned height,
		int      stride) :
		m_buf(0),
		m_rows(0),
		m_width(0),
		m_height(0),
		m_stride(0),
		m_max_height(0)
	{
		attach(buf, width, height, stride);
	}


	//------------------------------------------------------------------------
	void rendering_buffer::attach(unsigned char* buf,
		unsigned width,
		unsigned height,
		int      stride)
	{
		m_buf = buf;
		m_width = width;
		m_height = height;
		m_stride = stride;
		if (height > m_max_height)
		{
			delete[] m_rows;
			m_rows = new unsigned char*[m_max_height = height];
		}

		unsigned char* row_ptr = m_buf;

		if (stride < 0)
		{
			row_ptr = m_buf - int(height - 1) * stride;
		}

		unsigned char** rows = m_rows;

		while (height--)
		{
			*rows++ = row_ptr;
			row_ptr += stride;
		}
	}



}