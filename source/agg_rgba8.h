 
#ifndef AGG_RGBA8_INCLUDED
#define AGG_RGBA8_INCLUDED

#include "agg_define.h" 

namespace agg
{

    //========================================================================
    struct rgba8 
    {
        enum order { rgb, bgr };

        int8u r;
        int8u g;
        int8u b;
        int8u a;

        //--------------------------------------------------------------------
        rgba8() {}

        //--------------------------------------------------------------------
        rgba8(unsigned r_, unsigned g_, unsigned b_, unsigned a_=255) :
            r(int8u(r_)), g(int8u(g_)), b(int8u(b_)), a(int8u(a_)) {}

        //--------------------------------------------------------------------
        rgba8(unsigned packed, order o) : 
            r((o == rgb) ? ((packed >> 16) & 0xFF) : (packed & 0xFF)),
            g((packed >> 8)  & 0xFF),
            b((o == rgb) ? (packed & 0xFF) : ((packed >> 16) & 0xFF)),
            a(255) {}

        //--------------------------------------------------------------------
        void opacity(double a_)
        {
            if(a_ < 0.0) a_ = 0.0;
            if(a_ > 1.0) a_ = 1.0;
            a = int8u(a_ * 255.0);
        }

        //--------------------------------------------------------------------
        double opacity() const
        {
            return double(a) / 255.0;
        }

        //--------------------------------------------------------------------
        rgba8 gradient(rgba8 c, double k) const
        {
            rgba8 ret;
            int ik = int(k * 256);
            ret.r = int8u(int(r) + (((int(c.r) - int(r)) * ik) >> 8));
            ret.g = int8u(int(g) + (((int(c.g) - int(g)) * ik) >> 8));
            ret.b = int8u(int(b) + (((int(c.b) - int(b)) * ik) >> 8));
            ret.a = int8u(int(a) + (((int(c.a) - int(a)) * ik) >> 8));
            return ret;
        }

        rgba8 pre() const
        {
            return rgba8((r*a) >> 8, (g*a) >> 8, (b*a) >> 8, a);
        }
    };



 
}



#endif

