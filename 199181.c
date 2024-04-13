    template<typename tz, typename tc>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           const int x0, const int y0, const float z0,
                           const int x1, const int y1, const float z1,
                           const int x2, const int y2, const float z2,
                           const tc *const color, const float opacity=1,
                           const float brightness=1) {
      typedef typename cimg::superset<tz,float>::type tzfloat;
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      static const T maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max());
      const float
        nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f),
        nbrightness = cimg::cut(brightness,0,2);
      const longT whd = (longT)width()*height()*depth(), offx = spectrum()*whd;
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2;
      tzfloat nz0 = 1/(tzfloat)z0, nz1 = 1/(tzfloat)z1, nz2 = 1/(tzfloat)z2;
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,nz0,nz1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2,nz0,nz2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2,nz1,nz2);
      if (ny0>=height() || ny2<0) return *this;
      tzfloat
        pzl = (nz1 - nz0)/(ny1 - ny0),
        pzr = (nz2 - nz0)/(ny2 - ny0),
        pzn = (nz2 - nz1)/(ny2 - ny1),
        zr = ny0>=0?nz0:(nz0 - ny0*(nz2 - nz0)/(ny2 - ny0)),
        zl = ny1>=0?(ny0>=0?nz0:(nz0 - ny0*(nz1 - nz0)/(ny1 - ny0))):(pzl=pzn,(nz1 - ny1*(nz2 - nz1)/(ny2 - ny1)));
      _cimg_for_triangle1(*this,xleft0,xright0,y,nx0,ny0,nx1,ny1,nx2,ny2) {
        if (y==ny1) { zl = nz1; pzl = pzn; }
        int xleft = xleft0, xright = xright0;
        tzfloat zleft = zl, zright = zr;
        if (xright<xleft) cimg::swap(xleft,xright,zleft,zright);
        const int dx = xright - xleft;
        const tzfloat pentez = (zright - zleft)/dx;
        if (xleft<0 && dx) zleft-=xleft*(zright - zleft)/dx;
        if (xleft<0) xleft = 0;
        if (xright>=width() - 1) xright = width() - 1;
        T* ptrd = data(xleft,y,0,0);
        tz *ptrz = xleft<=xright?zbuffer.data(xleft,y):0;
        if (opacity>=1) {
          if (nbrightness==1) for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
              if (zleft>=(tzfloat)*ptrz) {
                *ptrz = (tz)zleft;
              const tc *col = color; cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=whd; }
              ptrd-=offx;
            }
            zleft+=pentez;
          } else if (nbrightness<1) for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
              if (zleft>=(tzfloat)*ptrz) {
                *ptrz = (tz)zleft;
              const tc *col = color; cimg_forC(*this,c) { *ptrd = (T)(nbrightness*(*col++)); ptrd+=whd; }
              ptrd-=offx;
            }
            zleft+=pentez;
          } else for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
              if (zleft>=(tzfloat)*ptrz) {
                *ptrz = (tz)zleft;
              const tc *col = color;
              cimg_forC(*this,c) { *ptrd = (T)((2 - nbrightness)**(col++) + (nbrightness - 1)*maxval); ptrd+=whd; }
              ptrd-=offx;
            }
            zleft+=pentez;
          }
        } else {
          if (nbrightness==1) for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
              if (zleft>=(tzfloat)*ptrz) {
                *ptrz = (tz)zleft;
              const tc *col = color; cimg_forC(*this,c) { *ptrd = (T)(nopacity**(col++) + *ptrd*copacity); ptrd+=whd; }
              ptrd-=offx;
            }
            zleft+=pentez;
          } else if (nbrightness<1) for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
              if (zleft>=(tzfloat)*ptrz) {
                *ptrz = (tz)zleft;
              const tc *col = color;
              cimg_forC(*this,c) { *ptrd = (T)(nopacity*nbrightness**(col++) + *ptrd*copacity); ptrd+=whd; }
              ptrd-=offx;
            }
            zleft+=pentez;
          } else for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
              if (zleft>=(tzfloat)*ptrz) {
                *ptrz = (tz)zleft;
              const tc *col = color;
              cimg_forC(*this,c) {
                const T val = (T)((2 - nbrightness)**(col++) + (nbrightness - 1)*maxval);
                *ptrd = (T)(nopacity*val + *ptrd*copacity);
                ptrd+=whd;
              }
              ptrd-=offx;
            }
            zleft+=pentez;
          }
        }
        zr+=pzr; zl+=pzl;
      }
      return *this;