    template<typename tc>
    CImg<T>& _draw_triangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const int x2, const int y2,
                            const tc *const color, const float opacity,
                            const float brightness) {
      cimg_init_scanline(color,opacity);
      const float nbrightness = cimg::cut(brightness,0,2);
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2;
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2);
      if (ny0<height() && ny2>=0) {
        if ((nx1 - nx0)*(ny2 - ny0) - (nx2 - nx0)*(ny1 - ny0)<0)
          _cimg_for_triangle1(*this,xl,xr,y,nx0,ny0,nx1,ny1,nx2,ny2)
            cimg_draw_scanline(xl,xr,y,color,opacity,nbrightness);
        else
          _cimg_for_triangle1(*this,xl,xr,y,nx0,ny0,nx1,ny1,nx2,ny2)
            cimg_draw_scanline(xr,xl,y,color,opacity,nbrightness);
      }
      return *this;