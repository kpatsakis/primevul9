    template<typename t, typename tc>
    CImg<T>& draw_line(const CImg<t>& points,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !points || points._width<2) return *this;
      bool ninit_hatch = init_hatch;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);

      case 2 : {
        const int x0 = (int)points(0,0), y0 = (int)points(0,1);
        int ox = x0, oy = y0;
        for (unsigned int i = 1; i<points._width; ++i) {
          const int x = (int)points(i,0), y = (int)points(i,1);
          draw_line(ox,oy,x,y,color,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y;
        }
      } break;
      default : {
        const int x0 = (int)points(0,0), y0 = (int)points(0,1), z0 = (int)points(0,2);
        int ox = x0, oy = y0, oz = z0;
        for (unsigned int i = 1; i<points._width; ++i) {
          const int x = (int)points(i,0), y = (int)points(i,1), z = (int)points(i,2);
          draw_line(ox,oy,oz,x,y,z,color,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y; oz = z;
        }
      }
      }
      return *this;