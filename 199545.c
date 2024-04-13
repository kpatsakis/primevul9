    template<typename tc>
    CImg<T>& draw_line(const int x0, const int y0, const int z0,
                       const int x1, const int y1, const int z1,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Specified color is (null).",
                                    cimg_instance);
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      int nx0 = x0, ny0 = y0, nz0 = z0, nx1 = x1, ny1 = y1, nz1 = z1;
      if (nx0>nx1) cimg::swap(nx0,nx1,ny0,ny1,nz0,nz1);
      if (nx1<0 || nx0>=width()) return *this;
      if (nx0<0) {
        const float D = 1.0f + nx1 - nx0;
        ny0-=(int)((float)nx0*(1.0f + ny1 - ny0)/D);
        nz0-=(int)((float)nx0*(1.0f + nz1 - nz0)/D);
        nx0 = 0;
      }
      if (nx1>=width()) {
        const float d = (float)nx1 - width(), D = 1.0f + nx1 - nx0;
        ny1+=(int)(d*(1.0f + ny0 - ny1)/D);
        nz1+=(int)(d*(1.0f + nz0 - nz1)/D);
        nx1 = width() - 1;
      }
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,nz0,nz1);
      if (ny1<0 || ny0>=height()) return *this;
      if (ny0<0) {
        const float D = 1.0f + ny1 - ny0;
        nx0-=(int)((float)ny0*(1.0f + nx1 - nx0)/D);
        nz0-=(int)((float)ny0*(1.0f + nz1 - nz0)/D);
        ny0 = 0;
      }
      if (ny1>=height()) {
        const float d = (float)ny1 - height(), D = 1.0f + ny1 - ny0;
        nx1+=(int)(d*(1.0f + nx0 - nx1)/D);
        nz1+=(int)(d*(1.0f + nz0 - nz1)/D);
        ny1 = height() - 1;
      }
      if (nz0>nz1) cimg::swap(nx0,nx1,ny0,ny1,nz0,nz1);
      if (nz1<0 || nz0>=depth()) return *this;
      if (nz0<0) {
        const float D = 1.0f + nz1 - nz0;
        nx0-=(int)((float)nz0*(1.0f + nx1 - nx0)/D);
        ny0-=(int)((float)nz0*(1.0f + ny1 - ny0)/D);
        nz0 = 0;
      }
      if (nz1>=depth()) {
        const float d = (float)nz1 - depth(), D = 1.0f + nz1 - nz0;
        nx1+=(int)(d*(1.0f + nx0 - nx1)/D);
        ny1+=(int)(d*(1.0f + ny0 - ny1)/D);
        nz1 = depth() - 1;
      }
      const unsigned int dmax = (unsigned int)cimg::max(cimg::abs(nx1 - nx0),cimg::abs(ny1 - ny0),nz1 - nz0);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const float px = (nx1 - nx0)/(float)dmax, py = (ny1 - ny0)/(float)dmax, pz = (nz1 - nz0)/(float)dmax;
      float x = (float)nx0, y = (float)ny0, z = (float)nz0;
      if (opacity>=1) for (unsigned int t = 0; t<=dmax; ++t) {
        if (!(~pattern) || (~pattern && pattern&hatch)) {
          T* ptrd = data((unsigned int)x,(unsigned int)y,(unsigned int)z);
          const tc *col = color; cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=whd; }
        }
        x+=px; y+=py; z+=pz; if (pattern) { hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1); }
      } else {
        const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
        for (unsigned int t = 0; t<=dmax; ++t) {
          if (!(~pattern) || (~pattern && pattern&hatch)) {
            T* ptrd = data((unsigned int)x,(unsigned int)y,(unsigned int)z);
            const tc *col = color; cimg_forC(*this,c) { *ptrd = (T)(*(col++)*nopacity + *ptrd*copacity); ptrd+=whd; }
          }
          x+=px; y+=py; z+=pz; if (pattern) { hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1); }
        }
      }
      return *this;