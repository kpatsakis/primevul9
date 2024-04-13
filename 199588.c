    template<typename t>
    CImg<T>& draw_spline(const int x0, const int y0, const float u0, const float v0,
                         const int x1, const int y1, const float u1, const float v1,
                         const CImg<t>& texture,
                         const int tx0, const int ty0, const int tx1, const int ty1,
                         const float opacity=1,
                         const float precision=4, const unsigned int pattern=~0U,
                         const bool init_hatch=true) {
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_empty()) return *this;
      if (is_overlapped(texture))
        return draw_spline(x0,y0,u0,v0,x1,y1,u1,v1,+texture,tx0,ty0,tx1,ty1,precision,opacity,pattern,init_hatch);
      if (x0==x1 && y0==y1)
        return draw_point(x0,y0,texture.get_vector_at(x0<=0?0:x0>=texture.width()?texture.width() - 1:x0,
                                                      y0<=0?0:y0>=texture.height()?texture.height() - 1:y0),opacity);
      bool ninit_hatch = init_hatch;
      const float
        ax = u0 + u1 + 2*(x0 - x1),
        bx = 3*(x1 - x0) - 2*u0 - u1,
        ay = v0 + v1 + 2*(y0 - y1),
        by = 3*(y1 - y0) - 2*v0 - v1,
        _precision = 1/(cimg::hypot((float)x0 - x1,(float)y0 - y1)*(precision>0?precision:1));
      int ox = x0, oy = y0, otx = tx0, oty = ty0;
      for (float t1 = 0; t1<1; t1+=_precision) {
        const float t2 = t1*t1, t3 = t2*t1;
        const int
          nx = (int)(ax*t3 + bx*t2 + u0*t1 + x0),
          ny = (int)(ay*t3 + by*t2 + v0*t1 + y0),
          ntx = tx0 + (int)((tx1 - tx0)*t1),
          nty = ty0 + (int)((ty1 - ty0)*t1);
        draw_line(ox,oy,nx,ny,texture,otx,oty,ntx,nty,opacity,pattern,ninit_hatch);
        ninit_hatch = false;
        ox = nx; oy = ny; otx = ntx; oty = nty;
      }
      return draw_line(ox,oy,x1,y1,texture,otx,oty,tx1,ty1,opacity,pattern,false);