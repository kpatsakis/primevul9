    template<typename tp, typename tt, typename tc>
    CImg<T>& draw_spline(const CImg<tp>& points, const CImg<tt>& tangents,
                         const tc *const color, const float opacity=1,
                         const bool is_closed_set=false, const float precision=4,
                         const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !points || !tangents || points._width<2 || tangents._width<2) return *this;
      bool ninit_hatch = init_hatch;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);

      case 2 : {
        const int x0 = (int)points(0,0), y0 = (int)points(0,1);
        const float u0 = (float)tangents(0,0), v0 = (float)tangents(0,1);
        int ox = x0, oy = y0;
        float ou = u0, ov = v0;
        for (unsigned int i = 1; i<points._width; ++i) {
          const int x = (int)points(i,0), y = (int)points(i,1);
          const float u = (float)tangents(i,0), v = (float)tangents(i,1);
          draw_spline(ox,oy,ou,ov,x,y,u,v,color,precision,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y; ou = u; ov = v;
        }
        if (is_closed_set) draw_spline(ox,oy,ou,ov,x0,y0,u0,v0,color,precision,opacity,pattern,false);
      } break;
      default : {
        const int x0 = (int)points(0,0), y0 = (int)points(0,1), z0 = (int)points(0,2);
        const float u0 = (float)tangents(0,0), v0 = (float)tangents(0,1), w0 = (float)tangents(0,2);
        int ox = x0, oy = y0, oz = z0;
        float ou = u0, ov = v0, ow = w0;
        for (unsigned int i = 1; i<points._width; ++i) {
          const int x = (int)points(i,0), y = (int)points(i,1), z = (int)points(i,2);
          const float u = (float)tangents(i,0), v = (float)tangents(i,1), w = (float)tangents(i,2);
          draw_spline(ox,oy,oz,ou,ov,ow,x,y,z,u,v,w,color,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y; oz = z; ou = u; ov = v; ow = w;
        }
        if (is_closed_set) draw_spline(ox,oy,oz,ou,ov,ow,x0,y0,z0,u0,v0,w0,color,precision,opacity,pattern,false);
      }
      }
      return *this;