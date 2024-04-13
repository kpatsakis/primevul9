    template<typename tc>
    CImg<T>& _draw_ellipse(const int x0, const int y0, const float r1, const float r2, const float angle,
                           const tc *const color, const float opacity,
                           const unsigned int pattern) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_ellipse(): Specified color is (null).",
                                    cimg_instance);
      if (r1<=0 || r2<=0) return draw_point(x0,y0,color,opacity);
      if (r1==r2 && (float)(int)r1==r1) {
        if (pattern) return draw_circle(x0,y0,r1,color,opacity,pattern);
        else return draw_circle(x0,y0,r1,color,opacity);
      }
      cimg_init_scanline(color,opacity);
      const float
        nr1 = cimg::abs(r1) - 0.5, nr2 = cimg::abs(r2) - 0.5,
        nangle = (float)(angle*cimg::PI/180),
        u = (float)std::cos(nangle),
        v = (float)std::sin(nangle),
        rmax = std::max(nr1,nr2),
        l1 = (float)std::pow(rmax/(nr1>0?nr1:1e-6),2),
        l2 = (float)std::pow(rmax/(nr2>0?nr2:1e-6),2),
        a = l1*u*u + l2*v*v,
        b = u*v*(l1 - l2),
        c = l1*v*v + l2*u*u;
      const int
        yb = (int)std::sqrt(a*rmax*rmax/(a*c - b*b)),
        tymin = y0 - yb - 1,
        tymax = y0 + yb + 1,
        ymin = tymin<0?0:tymin,
        ymax = tymax>=height()?height() - 1:tymax;
      int oxmin = 0, oxmax = 0;
      bool first_line = true;
      for (int y = ymin; y<=ymax; ++y) {
        const float
          Y = y - y0 + (y<y0?0.5f:-0.5f),
          delta = b*b*Y*Y - a*(c*Y*Y - rmax*rmax),
          sdelta = delta>0?(float)std::sqrt(delta)/a:0.0f,
          bY = b*Y/a,
          fxmin = x0 - 0.5f - bY - sdelta,
          fxmax = x0 + 0.5f - bY + sdelta;
        const int xmin = (int)cimg::round(fxmin), xmax = (int)cimg::round(fxmax);
        if (!pattern) cimg_draw_scanline(xmin,xmax,y,color,opacity,1);
        else {
          if (first_line) {
            if (y0 - yb>=0) cimg_draw_scanline(xmin,xmax,y,color,opacity,1);
            else draw_point(xmin,y,color,opacity).draw_point(xmax,y,color,opacity);
            first_line = false;
          } else {
            if (xmin<oxmin) cimg_draw_scanline(xmin,oxmin - 1,y,color,opacity,1);
            else cimg_draw_scanline(oxmin + (oxmin==xmin?0:1),xmin,y,color,opacity,1);
            if (xmax<oxmax) cimg_draw_scanline(xmax,oxmax - 1,y,color,opacity,1);
            else cimg_draw_scanline(oxmax + (oxmax==xmax?0:1),xmax,y,color,opacity,1);
            if (y==tymax) cimg_draw_scanline(xmin + 1,xmax - 1,y,color,opacity,1);
          }
        }
        oxmin = xmin; oxmax = xmax;
      }
      return *this;