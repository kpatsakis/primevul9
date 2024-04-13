    template<typename tc>
    CImg<T>& draw_circle(const int x0, const int y0, int radius,
                         const tc *const color, const float opacity,
                         const unsigned int pattern) {
      cimg::unused(pattern);
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_circle(): Specified color is (null).",
                                    cimg_instance);
      if (radius<0 || x0 - radius>=width() || y0 + radius<0 || y0 - radius>=height()) return *this;
      if (!radius) return draw_point(x0,y0,color,opacity);
      draw_point(x0 - radius,y0,color,opacity).draw_point(x0 + radius,y0,color,opacity).
        draw_point(x0,y0 - radius,color,opacity).draw_point(x0,y0 + radius,color,opacity);
      if (radius==1) return *this;
      for (int f = 1 - radius, ddFx = 0, ddFy = -(radius<<1), x = 0, y = radius; x<y; ) {
        if (f>=0) { f+=(ddFy+=2); --y; }
        ++x; ++(f+=(ddFx+=2));
        if (x!=y + 1) {
          const int x1 = x0 - y, x2 = x0 + y, y1 = y0 - x, y2 = y0 + x,
            x3 = x0 - x, x4 = x0 + x, y3 = y0 - y, y4 = y0 + y;
          draw_point(x1,y1,color,opacity).draw_point(x1,y2,color,opacity).
            draw_point(x2,y1,color,opacity).draw_point(x2,y2,color,opacity);
          if (x!=y)
            draw_point(x3,y3,color,opacity).draw_point(x4,y4,color,opacity).
              draw_point(x4,y3,color,opacity).draw_point(x3,y4,color,opacity);
        }
      }
      return *this;