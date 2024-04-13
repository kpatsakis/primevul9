    **/
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0, const int c0,
                            const int x1, const int y1, const int z1, const int c1,
                            const T val, const float opacity=1) {
      if (is_empty()) return *this;
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0,
        nz0 = z0<z1?z0:z1, nz1 = z0^z1^nz0,
        nc0 = c0<c1?c0:c1, nc1 = c0^c1^nc0;
      const int
        lX = (1 + nx1 - nx0) + (nx1>=width()?width() - 1 - nx1:0) + (nx0<0?nx0:0),
        lY = (1 + ny1 - ny0) + (ny1>=height()?height() - 1 - ny1:0) + (ny0<0?ny0:0),
        lZ = (1 + nz1 - nz0) + (nz1>=depth()?depth() - 1 - nz1:0) + (nz0<0?nz0:0),
        lC = (1 + nc1 - nc0) + (nc1>=spectrum()?spectrum() - 1 - nc1:0) + (nc0<0?nc0:0);
      const ulongT
        offX = (ulongT)_width - lX,
        offY = (ulongT)_width*(_height - lY),
        offZ = (ulongT)_width*_height*(_depth - lZ);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      T *ptrd = data(nx0<0?0:nx0,ny0<0?0:ny0,nz0<0?0:nz0,nc0<0?0:nc0);
      if (lX>0 && lY>0 && lZ>0 && lC>0)
        for (int v = 0; v<lC; ++v) {
          for (int z = 0; z<lZ; ++z) {
            for (int y = 0; y<lY; ++y) {
              if (opacity>=1) {
                if (sizeof(T)!=1) { for (int x = 0; x<lX; ++x) *(ptrd++) = val; ptrd+=offX; }
                else { std::memset(ptrd,(int)val,lX); ptrd+=_width; }
              } else { for (int x = 0; x<lX; ++x) { *ptrd = (T)(nopacity*val + *ptrd*copacity); ++ptrd; } ptrd+=offX; }
            }
            ptrd+=offY;
          }
          ptrd+=offZ;
        }
      return *this;