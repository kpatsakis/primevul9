    template<typename t, typename tc>
    CImg<T>& draw_polygon(const CImg<t>& points,
                          const tc *const color, const float opacity, const unsigned int pattern) {
      if (is_empty() || !points || points._width<3) return *this;
      bool ninit_hatch = true;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set.",
                                    cimg_instance);
      case 2 : { // 2d version.
        CImg<intT> npoints(points._width,2);
        int x = npoints(0,0) = (int)points(0,0), y = npoints(0,1) = (int)points(0,1);
        unsigned int nb_points = 1;
        for (unsigned int p = 1; p<points._width; ++p) {
          const int nx = (int)points(p,0), ny = (int)points(p,1);
          if (nx!=x || ny!=y) { npoints(nb_points,0) = nx; npoints(nb_points++,1) = ny; x = nx; y = ny; }
        }
        const int x0 = (int)npoints(0,0), y0 = (int)npoints(0,1);
        int ox = x0, oy = y0;
        for (unsigned int i = 1; i<nb_points; ++i) {
          const int x = (int)npoints(i,0), y = (int)npoints(i,1);
          draw_line(ox,oy,x,y,color,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y;
        }
        draw_line(ox,oy,x0,y0,color,opacity,pattern,false);
      } break;
      default : { // 3d version.
        CImg<intT> npoints(points._width,3);
        int
          x = npoints(0,0) = (int)points(0,0),
          y = npoints(0,1) = (int)points(0,1),
          z = npoints(0,2) = (int)points(0,2);
        unsigned int nb_points = 1;
        for (unsigned int p = 1; p<points._width; ++p) {
          const int nx = (int)points(p,0), ny = (int)points(p,1), nz = (int)points(p,2);
          if (nx!=x || ny!=y || nz!=z) {
            npoints(nb_points,0) = nx; npoints(nb_points,1) = ny; npoints(nb_points++,2) = nz;
            x = nx; y = ny; z = nz;
          }
        }
        const int x0 = (int)npoints(0,0), y0 = (int)npoints(0,1), z0 = (int)npoints(0,2);
        int ox = x0, oy = y0, oz = z0;
        for (unsigned int i = 1; i<nb_points; ++i) {
          const int x = (int)npoints(i,0), y = (int)npoints(i,1), z = (int)npoints(i,2);
          draw_line(ox,oy,oz,x,y,z,color,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y; oz = z;
        }
        draw_line(ox,oy,oz,x0,y0,z0,color,opacity,pattern,false);
      }
      }
      return *this;