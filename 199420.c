    template<typename tc, typename tl>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const CImg<tc>& texture,
                           const int tx0, const int ty0,
                           const int tx1, const int ty1,
                           const int tx2, const int ty2,
                           const CImg<tl>& light,
                           const int lx0, const int ly0,
                           const int lx1, const int ly1,
                           const int lx2, const int ly2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(x0,y0,x1,y1,x2,y2,texture,tx0,ty0,tx1,ty1,tx2,ty2,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      static const T maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max());
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        twh = (ulongT)texture._width*texture._height,
        lwh = (ulongT)light._width*light._height,
        offx = _spectrum*whd - 1;
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2,
        ntx0 = tx0, nty0 = ty0, ntx1 = tx1, nty1 = ty1, ntx2 = tx2, nty2 = ty2,
        nlx0 = lx0, nly0 = ly0, nlx1 = lx1, nly1 = ly1, nlx2 = lx2, nly2 = ly2;
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,ntx0,ntx1,nty0,nty1,nlx0,nlx1,nly0,nly1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2,ntx0,ntx2,nty0,nty2,nlx0,nlx2,nly0,nly2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2,ntx1,ntx2,nty1,nty2,nlx1,nlx2,nly1,nly2);
      if (ny0>=height() || ny2<0) return *this;
      const bool is_bump = texture._spectrum>=_spectrum + 2;
      const ulongT obx = twh*_spectrum, oby = twh*(_spectrum + 1);

      _cimg_for_triangle5(*this,xleft0,lxleft0,lyleft0,txleft0,tyleft0,xright0,lxright0,lyright0,txright0,tyright0,y,
                          nx0,ny0,nlx0,nly0,ntx0,nty0,nx1,ny1,nlx1,nly1,ntx1,nty1,nx2,ny2,nlx2,nly2,ntx2,nty2) {
        int
          xleft = xleft0, xright = xright0,
          lxleft = lxleft0, lxright = lxright0,
          lyleft = lyleft0, lyright = lyright0,
          txleft = txleft0, txright = txright0,
          tyleft = tyleft0, tyright = tyright0;
        if (xright<xleft) cimg::swap(xleft,xright,lxleft,lxright,lyleft,lyright,txleft,txright,tyleft,tyright);
        const int
          dx = xright - xleft,
          dlx = lxright>lxleft?lxright - lxleft:lxleft - lxright,
          dly = lyright>lyleft?lyright - lyleft:lyleft - lyright,
          dtx = txright>txleft?txright - txleft:txleft - txright,
          dty = tyright>tyleft?tyright - tyleft:tyleft - tyright,
          rlx = dx?(lxright - lxleft)/dx:0,
          rly = dx?(lyright - lyleft)/dx:0,
          rtx = dx?(txright - txleft)/dx:0,
          rty = dx?(tyright - tyleft)/dx:0,
          slx = lxright>lxleft?1:-1,
          sly = lyright>lyleft?1:-1,
          stx = txright>txleft?1:-1,
          sty = tyright>tyleft?1:-1,
          ndlx = dlx - (dx?dx*(dlx/dx):0),
          ndly = dly - (dx?dx*(dly/dx):0),
          ndtx = dtx - (dx?dx*(dtx/dx):0),
          ndty = dty - (dx?dx*(dty/dx):0);
        int errlx = dx>>1, errly = errlx, errtx = errlx, errty = errlx;
        if (xleft<0 && dx) {
          lxleft-=xleft*(lxright - lxleft)/dx;
          lyleft-=xleft*(lyright - lyleft)/dx;
          txleft-=xleft*(txright - txleft)/dx;
          tyleft-=xleft*(tyright - tyleft)/dx;
        }
        if (xleft<0) xleft = 0;
        if (xright>=width() - 1) xright = width() - 1;
        T* ptrd = data(xleft,y,0,0);
        if (opacity>=1) for (int x = xleft; x<=xright; ++x) {
          const tc *col = &texture._atXY(txleft,tyleft);
          const int bx = is_bump?128 - (int)col[obx]:0, by = is_bump?128 - (int)col[oby]:0;
          const tl *lig = &light._atXY(lxleft + bx,lyleft + by);
          cimg_forC(*this,c) {
            const tl l = *lig;
            *ptrd = (T)(l<1?l**col:(2 - l)**col + (l - 1)*maxval);
            ptrd+=whd; col+=twh; lig+=lwh;
          }
          ptrd-=offx;
          lxleft+=rlx+((errlx-=ndlx)<0?errlx+=dx,slx:0);
          lyleft+=rly+((errly-=ndly)<0?errly+=dx,sly:0);
          txleft+=rtx+((errtx-=ndtx)<0?errtx+=dx,stx:0);
          tyleft+=rty+((errty-=ndty)<0?errty+=dx,sty:0);
        } else for (int x = xleft; x<=xright; ++x) {
          const tc *col = &texture._atXY(txleft,tyleft);
          const int bx = is_bump?128 - (int)col[obx]:0, by = is_bump?128 - (int)col[oby]:0;
          const tl *lig = &light._atXY(lxleft + bx,lyleft + by);
          cimg_forC(*this,c) {
            const tl l = *lig;
            const T val = (T)(l<1?l**col:(2 - l)**col + (l - 1)*maxval);
            *ptrd = (T)(nopacity*val + *ptrd*copacity);
            ptrd+=whd; col+=twh; lig+=lwh;
          }
          ptrd-=offx;
          lxleft+=rlx+((errlx-=ndlx)<0?errlx+=dx,slx:0);
          lyleft+=rly+((errly-=ndly)<0?errly+=dx,sly:0);
          txleft+=rtx+((errtx-=ndtx)<0?errtx+=dx,stx:0);
          tyleft+=rty+((errty-=ndty)<0?errty+=dx,sty:0);
        }
      }
      return *this;