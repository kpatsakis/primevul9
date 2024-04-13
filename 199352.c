    template<typename tc, typename tl>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const tc *const color,
                           const CImg<tl>& light,
                           const int lx0, const int ly0,
                           const int lx1, const int ly1,
                           const int lx2, const int ly2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(light)) return draw_triangle(x0,y0,x1,y1,x2,y2,color,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      static const T maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max());
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2,
        nlx0 = lx0, nly0 = ly0, nlx1 = lx1, nly1 = ly1, nlx2 = lx2, nly2 = ly2;
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        lwh = (ulongT)light._width*light._height,
        offx = _spectrum*whd - 1;
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,nlx0,nlx1,nly0,nly1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2,nlx0,nlx2,nly0,nly2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2,nlx1,nlx2,nly1,nly2);
      if (ny0>=height() || ny2<0) return *this;
      _cimg_for_triangle3(*this,xleft0,lxleft0,lyleft0,xright0,lxright0,lyright0,y,
                          nx0,ny0,nlx0,nly0,nx1,ny1,nlx1,nly1,nx2,ny2,nlx2,nly2) {
        int
          xleft = xleft0, xright = xright0,
          lxleft = lxleft0, lxright = lxright0,
          lyleft = lyleft0, lyright = lyright0;
        if (xright<xleft) cimg::swap(xleft,xright,lxleft,lxright,lyleft,lyright);
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
        int errlx = dx>>1, errly = errlx;
        if (xleft<0 && dx) {
          lxleft-=xleft*(lxright - lxleft)/dx;
          lyleft-=xleft*(lyright - lyleft)/dx;
        }
        if (xleft<0) xleft = 0;
        if (xright>=width() - 1) xright = width() - 1;
        T* ptrd = data(xleft,y,0,0);
        if (opacity>=1) for (int x = xleft; x<=xright; ++x) {
          const tc *col = color;
          const tl *lig = &light._atXY(lxleft,lyleft);
          cimg_forC(*this,c) {
            const tl l = *lig;
            *ptrd = (T)(l<1?l**(col++):((2 - l)**(col++) + (l - 1)*maxval));
            ptrd+=whd; lig+=lwh;
          }
          ptrd-=offx;
          lxleft+=rlx+((errlx-=ndlx)<0?errlx+=dx,slx:0);
          lyleft+=rly+((errly-=ndly)<0?errly+=dx,sly:0);
        } else  for (int x = xleft; x<=xright; ++x) {
          const tc *col = color;
          const tl *lig = &light._atXY(lxleft,lyleft);
          cimg_forC(*this,c) {
            const tl l = *lig;
            const T val = (T)(l<1?l**(col++):((2 - l)**(col++) + (l - 1)*maxval));
            *ptrd = (T)(nopacity*val + *ptrd*copacity);
            ptrd+=whd; lig+=lwh;
          }
          ptrd-=offx;
          lxleft+=rlx+((errlx-=ndlx)<0?errlx+=dx,slx:0);
          lyleft+=rly+((errly-=ndly)<0?errly+=dx,sly:0);
        }
      }
      return *this;