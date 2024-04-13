    template<typename tc>
    CImg<T>& draw_arrow(const int x0, const int y0,
                        const int x1, const int y1,
                        const tc *const color, const float opacity=1,
                        const float angle=30, const float length=-10,
                        const unsigned int pattern=~0U) {
      if (is_empty()) return *this;
      const float u = (float)(x0 - x1), v = (float)(y0 - y1), sq = u*u + v*v,
        deg = (float)(angle*cimg::PI/180), ang = (sq>0)?(float)std::atan2(v,u):0.0f,
        l = (length>=0)?length:-length*(float)std::sqrt(sq)/100;
      if (sq>0) {
        const float
            cl = (float)std::cos(ang - deg), sl = (float)std::sin(ang - deg),
            cr = (float)std::cos(ang + deg), sr = (float)std::sin(ang + deg);
        const int
          xl = x1 + (int)(l*cl), yl = y1 + (int)(l*sl),
          xr = x1 + (int)(l*cr), yr = y1 + (int)(l*sr),
          xc = x1 + (int)((l + 1)*(cl + cr))/2, yc = y1 + (int)((l + 1)*(sl + sr))/2;
        draw_line(x0,y0,xc,yc,color,opacity,pattern).draw_triangle(x1,y1,xl,yl,xr,yr,color,opacity);
      } else draw_point(x0,y0,color,opacity);
      return *this;