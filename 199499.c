    template<typename tz,typename tc>
    CImg<T>& draw_line(CImg<tz>& zbuffer,
                       const int x0, const int y0, const float z0,
                       const int x1, const int y1, const float z1,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      typedef typename cimg::superset<tz,float>::type tzfloat;
      if (is_empty() || z0<=0 || z1<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Specified color is (null).",
                                    cimg_instance);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      const bool xdir = x0<x1, ydir = y0<y1;
      int
        nx0 = x0, nx1 = x1, ny0 = y0, ny1 = y1,
        &xleft = xdir?nx0:nx1, &yleft = xdir?ny0:ny1,
        &xright = xdir?nx1:nx0, &yright = xdir?ny1:ny0,
        &xup = ydir?nx0:nx1, &yup = ydir?ny0:ny1,
        &xdown = ydir?nx1:nx0, &ydown = ydir?ny1:ny0;
      tzfloat
        Z0 = 1/(tzfloat)z0, Z1 = 1/(tzfloat)z1, nz0 = Z0, nz1 = Z1, dz = Z1 - Z0,
        &zleft = xdir?nz0:nz1,
        &zright = xdir?nz1:nz0,
        &zup = ydir?nz0:nz1,
        &zdown = ydir?nz1:nz0;
      if (xright<0 || xleft>=width()) return *this;
      if (xleft<0) {
        const float D = (float)xright - xleft;
        yleft-=(int)((float)xleft*((float)yright - yleft)/D);
        zleft-=(tzfloat)xleft*(zright - zleft)/D;
        xleft = 0;
      }
      if (xright>=width()) {
        const float d = (float)xright - width(), D = (float)xright - xleft;
        yright-=(int)(d*((float)yright - yleft)/D);
        zright-=(tzfloat)d*(zright - zleft)/D;
        xright = width() - 1;
      }
      if (ydown<0 || yup>=height()) return *this;
      if (yup<0) {
        const float D = (float)ydown - yup;
        xup-=(int)((float)yup*((float)xdown - xup)/D);
        zup-=(tzfloat)yup*(zdown - zup)/D;
        yup = 0;
      }
      if (ydown>=height()) {
        const float d = (float)ydown - height(), D = (float)ydown - yup;
        xdown-=(int)(d*((float)xdown - xup)/D);
        zdown-=(tzfloat)d*(zdown - zup)/D;
        ydown = height() - 1;
      }
      T *ptrd0 = data(nx0,ny0);
      tz *ptrz = zbuffer.data(nx0,ny0);
      int dx = xright - xleft, dy = ydown - yup;
      const bool steep = dy>dx;
      if (steep) cimg::swap(nx0,ny0,nx1,ny1,dx,dy);
      const longT
        offx = (longT)(nx0<nx1?1:-1)*(steep?width():1),
        offy = (longT)(ny0<ny1?1:-1)*(steep?1:width());
      const ulongT
        wh = (ulongT)_width*_height,
        ndx = (ulongT)(dx>0?dx:1);
      if (opacity>=1) {
        if (~pattern) for (int error = dx>>1, x = 0; x<=dx; ++x) {
          const tzfloat z = Z0 + x*dz/ndx;
          if (z>=(tzfloat)*ptrz && pattern&hatch) {
            *ptrz = (tz)z;
            T *ptrd = ptrd0; const tc *col = color;
            cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=wh; }
          }
          hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1);
          ptrd0+=offx; ptrz+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; ptrz+=offy; error+=dx; }
        } else for (int error = dx>>1, x = 0; x<=dx; ++x) {
          const tzfloat z = Z0 + x*dz/ndx;
          if (z>=(tzfloat)*ptrz) {
            *ptrz = (tz)z;
            T *ptrd = ptrd0; const tc *col = color;
            cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=wh; }
          }
          ptrd0+=offx; ptrz+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; ptrz+=offy; error+=dx; }
        }
      } else {
        const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
        if (~pattern) for (int error = dx>>1, x = 0; x<=dx; ++x) {
          const tzfloat z = Z0 + x*dz/ndx;
          if (z>=(tzfloat)*ptrz && pattern&hatch) {
            *ptrz = (tz)z;
            T *ptrd = ptrd0; const tc *col = color;
            cimg_forC(*this,c) { *ptrd = (T)(nopacity**(col++) + *ptrd*copacity); ptrd+=wh; }
          }
          hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1);
          ptrd0+=offx; ptrz+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; ptrz+=offy; error+=dx; }
        } else for (int error = dx>>1, x = 0; x<=dx; ++x) {
          const tzfloat z = Z0 + x*dz/ndx;
          if (z>=(tzfloat)*ptrz) {
            *ptrz = (tz)z;
            T *ptrd = ptrd0; const tc *col = color;
            cimg_forC(*this,c) { *ptrd = (T)(nopacity**(col++) + *ptrd*copacity); ptrd+=wh; }
          }
          ptrd0+=offx; ptrz+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; ptrz+=offy; error+=dx; }
        }
      }
      return *this;