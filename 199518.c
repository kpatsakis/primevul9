    template<typename tz, typename tc, typename tl>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           const int x0, const int y0, const float z0,
                           const int x1, const int y1, const float z1,
                           const int x2, const int y2, const float z2,
                           const tc *const color,
                           const CImg<tl>& light,
                           const int lx0, const int ly0,
                           const int lx1, const int ly1,
                           const int lx2, const int ly2,
                           const float opacity=1) {
      typedef typename cimg::superset<tz,float>::type tzfloat;
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (is_overlapped(light)) return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,
                                                     +light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      static const T maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max());
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        lwh = (ulongT)light._width*light._height,
        offx = _spectrum*whd;
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2,
        nlx0 = lx0, nly0 = ly0, nlx1 = lx1, nly1 = ly1, nlx2 = lx2, nly2 = ly2;
      tzfloat nz0 = 1/(tzfloat)z0, nz1 = 1/(tzfloat)z1, nz2 = 1/(tzfloat)z2;
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,nlx0,nlx1,nly0,nly1,nz0,nz1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2,nlx0,nlx2,nly0,nly2,nz0,nz2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2,nlx1,nlx2,nly1,nly2,nz1,nz2);
      if (ny0>=height() || ny2<0) return *this;
      tzfloat
        pzl = (nz1 - nz0)/(ny1 - ny0),
        pzr = (nz2 - nz0)/(ny2 - ny0),
        pzn = (nz2 - nz1)/(ny2 - ny1),
        zr = ny0>=0?nz0:(nz0 - ny0*(nz2 - nz0)/(ny2 - ny0)),
        zl = ny1>=0?(ny0>=0?nz0:(nz0 - ny0*(nz1 - nz0)/(ny1 - ny0))):(pzl=pzn,(nz1 - ny1*(nz2 - nz1)/(ny2 - ny1)));
      _cimg_for_triangle3(*this,xleft0,lxleft0,lyleft0,xright0,lxright0,lyright0,y,
                          nx0,ny0,nlx0,nly0,nx1,ny1,nlx1,nly1,nx2,ny2,nlx2,nly2) {
        if (y==ny1) { zl = nz1; pzl = pzn; }
        int
          xleft = xleft0, xright = xright0,
          lxleft = lxleft0, lxright = lxright0,
          lyleft = lyleft0, lyright = lyright0;
        tzfloat zleft = zl, zright = zr;
        if (xright<xleft) cimg::swap(xleft,xright,zleft,zright,lxleft,lxright,lyleft,lyright);
        const int
          dx = xright - xleft,
          dlx = lxright>lxleft?lxright - lxleft:lxleft - lxright,
          dly = lyright>lyleft?lyright - lyleft:lyleft - lyright,
          rlx = dx?(lxright - lxleft)/dx:0,
          rly = dx?(lyright - lyleft)/dx:0,
          slx = lxright>lxleft?1:-1,
          sly = lyright>lyleft?1:-1,
          ndlx = dlx - (dx?dx*(dlx/dx):0),
          ndly = dly - (dx?dx*(dly/dx):0);
        const tzfloat pentez = (zright - zleft)/dx;
        int errlx = dx>>1, errly = errlx;
        if (xleft<0 && dx) {
          zleft-=xleft*(zright - zleft)/dx;
          lxleft-=xleft*(lxright - lxleft)/dx;
          lyleft-=xleft*(lyright - lyleft)/dx;
        }
        if (xleft<0) xleft = 0;
        if (xright>=width() - 1) xright = width() - 1;
        T *ptrd = data(xleft,y,0,0);
        tz *ptrz = xleft<=xright?zbuffer.data(xleft,y):0;
        if (opacity>=1) for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
            if (zleft>=(tzfloat)*ptrz) {
              *ptrz = (tz)zleft;
              const tc *col = color;
              const tl *lig = &light._atXY(lxleft,lyleft);
              cimg_forC(*this,c) {
                const tl l = *lig;
                const tc cval = *(col++);
                *ptrd = (T)(l<1?l*cval:(2 - l)*cval + (l - 1)*maxval);
                ptrd+=whd; lig+=lwh;
              }
              ptrd-=offx;
            }
            zleft+=pentez;
            lxleft+=rlx+((errlx-=ndlx)<0?errlx+=dx,slx:0);
            lyleft+=rly+((errly-=ndly)<0?errly+=dx,sly:0);
          } else for (int x = xleft; x<=xright; ++x, ++ptrz, ++ptrd) {
            if (zleft>=(tzfloat)*ptrz) {
              *ptrz = (tz)zleft;
              const tc *col = color;
              const tl *lig = &light._atXY(lxleft,lyleft);
              cimg_forC(*this,c) {
                const tl l = *lig;
                const tc cval = *(col++);
                const T val = (T)(l<1?l*cval:(2 - l)*cval + (l - 1)*maxval);
                *ptrd = (T)(nopacity*val + *ptrd*copacity);
                ptrd+=whd; lig+=lwh;
              }
              ptrd-=offx;
            }
            zleft+=pentez;
            lxleft+=rlx+((errlx-=ndlx)<0?errlx+=dx,slx:0);
            lyleft+=rly+((errly-=ndly)<0?errly+=dx,sly:0);
          }
        zr+=pzr; zl+=pzl;
      }
      return *this;