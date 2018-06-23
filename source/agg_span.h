 
#ifndef AGG_SPAN_INCLUDED
#define AGG_SPAN_INCLUDED

#include "agg_rgba8.h"

namespace agg
{
	//========================================================================
	struct span_mono8
	{
		//--------------------------------------------------------------------
		static unsigned mono8(unsigned r, unsigned g, unsigned b)
		{
			return (r * 77 + g * 150 + b * 29) >> 8;
		}

		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + x;
			unsigned dst = mono8(c.r, c.g, c.b);
			do
			{
				int alpha = (*covers++) * c.a;
				unsigned src = *p;
				*p++ = (((dst - src) * alpha) + (src << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + x;
			unsigned v = mono8(c.r, c.g, c.b);
			do { *p++ = v; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned rgb = ptr[x];
			rgba8 c;
			c.r = rgb;
			c.g = rgb;
			c.b = rgb;
			c.a = 255;
			return c;
		}
	};


	//========================================================================
	struct span_rgb555
	{
		//--------------------------------------------------------------------
		static int16u rgb555(unsigned r, unsigned g, unsigned b)
		{
			return ((r & 0xF8) << 7) | ((g & 0xF8) << 2) | (b >> 3);
		}

		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			int16u* p = ((int16u*)ptr) + x;
			do
			{
				int16 rgb = *p;
				int alpha = (*covers++) * c.a;

				int r = (rgb >> 7) & 0xF8;
				int g = (rgb >> 2) & 0xF8;
				int b = (rgb << 3) & 0xF8;

				*p++ = (((((c.r - r) * alpha) + (r << 16)) >> 9) & 0x7C00) |
					(((((c.g - g) * alpha) + (g << 16)) >> 14) & 0x3E0) |
					((((c.b - b) * alpha) + (b << 16)) >> 19);
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			int16u* p = ((int16u*)ptr) + x;
			int16u  v = rgb555(c.r, c.g, c.b);
			do { *p++ = v; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			int16u rgb = ((int16u*)ptr)[x];
			rgba8 c;
			c.r = (rgb >> 7) & 0xF8;
			c.g = (rgb >> 2) & 0xF8;
			c.b = (rgb << 3) & 0xF8;
			c.a = 255;
			return c;
		}
	};




	//========================================================================
	struct span_rgb565
	{
		//--------------------------------------------------------------------
		static int16u rgb565(unsigned r, unsigned g, unsigned b)
		{
			return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
		}

		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			int16u* p = ((int16u*)ptr) + x;
			do
			{
				int16 rgb = *p;
				int alpha = (*covers++) * c.a;

				int r = (rgb >> 8) & 0xF8;
				int g = (rgb >> 3) & 0xFC;
				int b = (rgb << 3) & 0xF8;

				*p++ = (((((c.r - r) * alpha) + (r << 16)) >> 8) & 0xF800) |
					(((((c.g - g) * alpha) + (g << 16)) >> 13) & 0x7E0) |
					((((c.b - b) * alpha) + (b << 16)) >> 19);
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			int16u* p = ((int16u*)ptr) + x;
			int16u  v = rgb565(c.r, c.g, c.b);
			do { *p++ = v; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			int16u rgb = ((int16u*)ptr)[x];
			rgba8 c;
			c.r = (rgb >> 8) & 0xF8;
			c.g = (rgb >> 3) & 0xFC;
			c.b = (rgb << 3) & 0xF8;
			c.a = 255;
			return c;
		}
	};



	//========================================================================
	struct span_bgr24
	{
		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + x + x + x;
			do
			{
				int alpha = (*covers++) * c.a;
				int b = p[0];
				int g = p[1];
				int r = p[2];
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + x + x + x;
			do { *p++ = c.b; *p++ = c.g; *p++ = c.r; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned char* p = ptr + x + x + x;
			rgba8 c;
			c.b = *p++;
			c.g = *p++;
			c.r = *p++;
			c.a = 255;
			return c;
		}
	};



	//========================================================================
	struct span_rgb24
	{
		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + x + x + x;
			do
			{
				int alpha = (*covers++) * c.a;
				int r = p[0];
				int g = p[1];
				int b = p[2];
				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + x + x + x;
			do { *p++ = c.r; *p++ = c.g; *p++ = c.b; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned char* p = ptr + x + x + x;
			rgba8 c;
			c.r = *p++;
			c.g = *p++;
			c.b = *p++;
			c.a = 255;
			return c;
		}
	};



	//========================================================================
	struct span_abgr32
	{
		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do
			{
				int alpha = (*covers++) * c.a;
				int a = p[0];
				int b = p[1];
				int g = p[2];
				int r = p[3];
				*p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do { *p++ = c.a; *p++ = c.b; *p++ = c.g; *p++ = c.r; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned char* p = ptr + (x << 2);
			rgba8 c;
			c.a = *p++;
			c.b = *p++;
			c.g = *p++;
			c.r = *p;
			return c;
		}
	};




	//========================================================================
	struct span_argb32
	{
		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do
			{
				int alpha = (*covers++) * c.a;
				int a = p[0];
				int r = p[1];
				int g = p[2];
				int b = p[3];
				*p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;
				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do { *p++ = c.a; *p++ = c.r; *p++ = c.g; *p++ = c.b; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned char* p = ptr + (x << 2);
			rgba8 c;
			c.a = *p++;
			c.r = *p++;
			c.g = *p++;
			c.b = *p;
			return c;
		}
	};



	//========================================================================
	struct span_bgra32
	{
		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do
			{
				int alpha = (*covers++) * c.a;
				int b = p[0];
				int g = p[1];
				int r = p[2];
				int a = p[3];
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
				*p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do { *p++ = c.b; *p++ = c.g; *p++ = c.r; *p++ = c.a; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned char* p = ptr + (x << 2);
			rgba8 c;
			c.b = *p++;
			c.g = *p++;
			c.r = *p++;
			c.a = *p;
			return c;
		}
	};




	//========================================================================
	struct span_rgba32
	{
		//--------------------------------------------------------------------
		static void render(unsigned char* ptr,
			int x,
			unsigned count,
			const unsigned char* covers,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do
			{
				int alpha = (*covers++) * c.a;
				int r = p[0];
				int g = p[1];
				int b = p[2];
				int a = p[3];
				*p++ = (((c.r - r) * alpha) + (r << 16)) >> 16;
				*p++ = (((c.g - g) * alpha) + (g << 16)) >> 16;
				*p++ = (((c.b - b) * alpha) + (b << 16)) >> 16;
				*p++ = (((c.a - a) * alpha) + (a << 16)) >> 16;
			} while (--count);
		}

		//--------------------------------------------------------------------
		static void hline(unsigned char* ptr,
			int x,
			unsigned count,
			const rgba8& c)
		{
			unsigned char* p = ptr + (x << 2);
			do { *p++ = c.r; *p++ = c.g; *p++ = c.b; *p++ = c.a; } while (--count);
		}

		//--------------------------------------------------------------------
		static rgba8 get(unsigned char* ptr, int x)
		{
			unsigned char* p = ptr + (x << 2);
			rgba8 c;
			c.r = *p++;
			c.g = *p++;
			c.b = *p++;
			c.a = *p;
			return c;
		}
	};


}



#endif  //AGG_DEFINE_INCLUDED

