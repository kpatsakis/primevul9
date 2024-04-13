    template<typename tp, typename tc>
    CImg<T>& draw_spline(const CImg<tp>& points,
                         const tc *const color, const float opacity=1,
                         const bool is_closed_set=false, const float precision=4,
                         const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !points || points._width<2) return *this;
      CImg<Tfloat> tangents;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);
      case 2 : {
        tangents.assign(points._width,points._height);
        cimg_forX(points,p) {
          const unsigned int
            p0 = is_closed_set?(p + points._width - 1)%points._width:(p?p - 1:0),
            p1 = is_closed_set?(p + 1)%points._width:(p + 1<points._width?p + 1:p);
          const float
            x = (float)points(p,0),
            y = (float)points(p,1),
            x0 = (float)points(p0,0),
            y0 = (float)points(p0,1),
            x1 = (float)points(p1,0),
            y1 = (float)points(p1,1),
            u0 = x - x0,
            v0 = y - y0,
            n0 = 1e-8f + cimg::hypot(u0,v0),
            u1 = x1 - x,
            v1 = y1 - y,
            n1 = 1e-8f + cimg::hypot(u1,v1),
            u = u0/n0 + u1/n1,
            v = v0/n0 + v1/n1,
            n = 1e-8f + cimg::hypot(u,v),
            fact = 0.5f*(n0 + n1);
          tangents(p,0) = (Tfloat)(fact*u/n);
          tangents(p,1) = (Tfloat)(fact*v/n);
        }
      } break;
      default : {
        tangents.assign(points._width,points._height);
        cimg_forX(points,p) {
          const unsigned int
            p0 = is_closed_set?(p + points._width - 1)%points._width:(p?p - 1:0),
            p1 = is_closed_set?(p + 1)%points._width:(p + 1<points._width?p + 1:p);
          const float
            x = (float)points(p,0),
            y = (float)points(p,1),
            z = (float)points(p,2),
            x0 = (float)points(p0,0),
            y0 = (float)points(p0,1),
            z0 = (float)points(p0,2),
            x1 = (float)points(p1,0),
            y1 = (float)points(p1,1),
            z1 = (float)points(p1,2),
            u0 = x - x0,
            v0 = y - y0,
            w0 = z - z0,
            n0 = 1e-8f + cimg::hypot(u0,v0,w0),
            u1 = x1 - x,
            v1 = y1 - y,
            w1 = z1 - z,
            n1 = 1e-8f + cimg::hypot(u1,v1,w1),
            u = u0/n0 + u1/n1,
            v = v0/n0 + v1/n1,
            w = w0/n0 + w1/n1,
            n = 1e-8f + cimg::hypot(u,v,w),
            fact = 0.5f*(n0 + n1);
          tangents(p,0) = (Tfloat)(fact*u/n);
          tangents(p,1) = (Tfloat)(fact*v/n);
          tangents(p,2) = (Tfloat)(fact*w/n);
        }
      }
      }
      return draw_spline(points,tangents,color,opacity,is_closed_set,precision,pattern,init_hatch);