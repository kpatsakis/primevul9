    template<typename tc>
    CImg<T>& draw_circle(const int x0, const int y0, int radius,
                         const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_circle(): Specified color is (null).",
                                    cimg_instance);
      cimg_init_scanline(color,opacity);
      if (radius<0 || x0 - radius>=width() || y0 + radius<0 || y0 - radius>=height()) return *this;
      if (y0>=0 && y0<height()) cimg_draw_scanline(x0 - radius,x0 + radius,y0,color,opacity,1);
      for (int f = 1 - radius, ddFx = 0, ddFy = -(radius<<1), x = 0, y = radius; x<y; ) {
        if (f>=0) {
          const int x1 = x0 - x, x2 = x0 + x, y1 = y0 - y, y2 = y0 + y;
          if (y1>=0 && y1<height()) cimg_draw_scanline(x1,x2,y1,color,opacity,1);
          if (y2>=0 && y2<height()) cimg_draw_scanline(x1,x2,y2,color,opacity,1);
          f+=(ddFy+=2); --y;
        }
        const bool no_diag = y!=(x++);
        ++(f+=(ddFx+=2));
        const int x1 = x0 - y, x2 = x0 + y, y1 = y0 - x, y2 = y0 + x;
        if (no_diag) {
          if (y1>=0 && y1<height()) cimg_draw_scanline(x1,x2,y1,color,opacity,1);
          if (y2>=0 && y2<height()) cimg_draw_scanline(x1,x2,y2,color,opacity,1);
        }
      }
      return *this;