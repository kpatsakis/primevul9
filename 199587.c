    template<typename tc>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const CImg<tc>& texture,
                           const int tx0, const int ty0,
                           const int tx1, const int ty1,
                           const int tx2, const int ty2,
                           const float brightness0,
                           const float brightness1,
                           const float brightness2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             brightness0,brightness1,brightness2,opacity);
      static const T maxval = (T)std::min(cimg::type<T>::max(),cimg::type<tc>::max());
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        twh = (ulongT)texture._width*texture._height,
        offx = _spectrum*whd - 1;
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2,
        ntx0 = tx0, nty0 = ty0, ntx1 = tx1, nty1 = ty1, ntx2 = tx2, nty2 = ty2,
        nc0 = (int)((brightness0<0.0f?0.0f:(brightness0>2.0f?2.0f:brightness0))*256.0f),
        nc1 = (int)((brightness1<0.0f?0.0f:(brightness1>2.0f?2.0f:brightness1))*256.0f),
        nc2 = (int)((brightness2<0.0f?0.0f:(brightness2>2.0f?2.0f:brightness2))*256.0f);
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,ntx0,ntx1,nty0,nty1,nc0,nc1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2,ntx0,ntx2,nty0,nty2,nc0,nc2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2,ntx1,ntx2,nty1,nty2,nc1,nc2);
      if (ny0>=height() || ny2<0) return *this;
      _cimg_for_triangle4(*this,xleft0,cleft0,txleft0,tyleft0,xright0,cright0,txright0,tyright0,y,
                          nx0,ny0,nc0,ntx0,nty0,nx1,ny1,nc1,ntx1,nty1,nx2,ny2,nc2,ntx2,nty2) {
        int
          xleft = xleft0, xright = xright0,
          cleft = cleft0, cright = cright0,
          txleft = txleft0, txright = txright0,
          tyleft = tyleft0, tyright = tyright0;
        if (xright<xleft) cimg::swap(xleft,xright,cleft,cright,txleft,txright,tyleft,tyright);
        const int
          dx = xright - xleft,
          dc = cright>cleft?cright - cleft:cleft - cright,
          dtx = txright>txleft?txright - txleft:txleft - txright,
          dty = tyright>tyleft?tyright - tyleft:tyleft - tyright,
          rc = dx?(cright - cleft)/dx:0,
          rtx = dx?(txright - txleft)/dx:0,
          rty = dx?(tyright - tyleft)/dx:0,
          sc = cright>cleft?1:-1,
          stx = txright>txleft?1:-1,
          sty = tyright>tyleft?1:-1,
          ndc = dc - (dx?dx*(dc/dx):0),
          ndtx = dtx - (dx?dx*(dtx/dx):0),
          ndty = dty - (dx?dx*(dty/dx):0);
        int errc = dx>>1, errtx = errc, errty = errc;
        if (xleft<0 && dx) {
          cleft-=xleft*(cright - cleft)/dx;
          txleft-=xleft*(txright - txleft)/dx;
          tyleft-=xleft*(tyright - tyleft)/dx;
        }
        if (xleft<0) xleft = 0;
        if (xright>=width() - 1) xright = width() - 1;
        T* ptrd = data(xleft,y,0,0);
        if (opacity>=1) for (int x = xleft; x<=xright; ++x) {
          const tc *col = &texture._atXY(txleft,tyleft);
          cimg_forC(*this,c) {
            *ptrd = (T)(cleft<256?cleft**col/256:((512 - cleft)**col + (cleft - 256)*maxval)/256);
            ptrd+=whd; col+=twh;
          }
          ptrd-=offx;
          cleft+=rc+((errc-=ndc)<0?errc+=dx,sc:0);
          txleft+=rtx+((errtx-=ndtx)<0?errtx+=dx,stx:0);
          tyleft+=rty+((errty-=ndty)<0?errty+=dx,sty:0);
        } else for (int x = xleft; x<=xright; ++x) {
          const tc *col = &texture._atXY(txleft,tyleft);
          cimg_forC(*this,c) {
            const T val = (T)(cleft<256?cleft**col/256:((512 - cleft)**col + (cleft - 256)*maxval)/256);
            *ptrd = (T)(nopacity*val + *ptrd*copacity);
            ptrd+=whd; col+=twh;
          }
          ptrd-=offx;
          cleft+=rc+((errc-=ndc)<0?errc+=dx,sc:0);
          txleft+=rtx+((errtx-=ndtx)<0?errtx+=dx,stx:0);
          tyleft+=rty+((errty-=ndty)<0?errty+=dx,sty:0);
        }
      }
      return *this;