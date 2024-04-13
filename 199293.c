    template<typename tc>
    CImg<T>& draw_triangle(const int x0, const int y0, const float z0,
                           const int x1, const int y1, const float z1,
                           const int x2, const int y2, const float z2,
                           const CImg<tc>& texture,
                           const int tx0, const int ty0,
                           const int tx1, const int ty1,
                           const int tx2, const int ty2,
                           const float opacity=1,
                           const float brightness=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,opacity,brightness);
      static const T maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max());
      const float
        nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f),
        nbrightness = cimg::cut(brightness,0,2);
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        twh = (ulongT)texture._width*texture._height,
        offx = _spectrum*whd - 1;
      int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2;
      float
        ntx0 = tx0/z0, nty0 = ty0/z0,
        ntx1 = tx1/z1, nty1 = ty1/z1,
        ntx2 = tx2/z2, nty2 = ty2/z2,
        nz0 = 1/z0, nz1 = 1/z1, nz2 = 1/z2;
      if (ny0>ny1) cimg::swap(nx0,nx1,ny0,ny1,ntx0,ntx1,nty0,nty1,nz0,nz1);
      if (ny0>ny2) cimg::swap(nx0,nx2,ny0,ny2,ntx0,ntx2,nty0,nty2,nz0,nz2);
      if (ny1>ny2) cimg::swap(nx1,nx2,ny1,ny2,ntx1,ntx2,nty1,nty2,nz1,nz2);
      if (ny0>=height() || ny2<0) return *this;
      float
        ptxl = (ntx1 - ntx0)/(ny1 - ny0),
        ptxr = (ntx2 - ntx0)/(ny2 - ny0),
        ptxn = (ntx2 - ntx1)/(ny2 - ny1),
        ptyl = (nty1 - nty0)/(ny1 - ny0),
        ptyr = (nty2 - nty0)/(ny2 - ny0),
        ptyn = (nty2 - nty1)/(ny2 - ny1),
        pzl = (nz1 - nz0)/(ny1 - ny0),
        pzr = (nz2 - nz0)/(ny2 - ny0),
        pzn = (nz2 - nz1)/(ny2 - ny1),
        zr = ny0>=0?nz0:(nz0 - ny0*(nz2 - nz0)/(ny2 - ny0)),
        txr = ny0>=0?ntx0:(ntx0 - ny0*(ntx2 - ntx0)/(ny2 - ny0)),
        tyr = ny0>=0?nty0:(nty0 - ny0*(nty2 - nty0)/(ny2 - ny0)),
        zl = ny1>=0?(ny0>=0?nz0:(nz0 - ny0*(nz1 - nz0)/(ny1 - ny0))):(pzl=pzn,(nz1 - ny1*(nz2 - nz1)/(ny2 - ny1))),
        txl = ny1>=0?(ny0>=0?ntx0:(ntx0 - ny0*(ntx1 - ntx0)/(ny1 - ny0))):
          (ptxl=ptxn,(ntx1 - ny1*(ntx2 - ntx1)/(ny2 - ny1))),
        tyl = ny1>=0?(ny0>=0?nty0:(nty0 - ny0*(nty1 - nty0)/(ny1 - ny0))):
          (ptyl=ptyn,(nty1 - ny1*(nty2 - nty1)/(ny2 - ny1)));
      _cimg_for_triangle1(*this,xleft0,xright0,y,nx0,ny0,nx1,ny1,nx2,ny2) {
        if (y==ny1) { zl = nz1; txl = ntx1; tyl = nty1; pzl = pzn; ptxl = ptxn; ptyl = ptyn; }
        int xleft = xleft0, xright = xright0;
        float
          zleft = zl, zright = zr,
          txleft = txl, txright = txr,
          tyleft = tyl, tyright = tyr;
        if (xright<xleft) cimg::swap(xleft,xright,zleft,zright,txleft,txright,tyleft,tyright);
        const int dx = xright - xleft;
        const float
          pentez = (zright - zleft)/dx,
          pentetx = (txright - txleft)/dx,
          pentety = (tyright - tyleft)/dx;
        if (xleft<0 && dx) {
          zleft-=xleft*(zright - zleft)/dx;
          txleft-=xleft*(txright - txleft)/dx;
          tyleft-=xleft*(tyright - tyleft)/dx;
        }
        if (xleft<0) xleft = 0;
        if (xright>=width() - 1) xright = width() - 1;
        T* ptrd = data(xleft,y,0,0);
        if (opacity>=1) {
          if (nbrightness==1) for (int x = xleft; x<=xright; ++x) {
            const float invz = 1/zleft;
            const tc *col = &texture._atXY((int)(txleft*invz),(int)(tyleft*invz));
            cimg_forC(*this,c) {
              *ptrd = (T)*col;
              ptrd+=whd; col+=twh;
            }
            ptrd-=offx; zleft+=pentez; txleft+=pentetx; tyleft+=pentety;
          } else if (nbrightness<1) for (int x=xleft; x<=xright; ++x) {
            const float invz = 1/zleft;
            const tc *col = &texture._atXY((int)(txleft*invz),(int)(tyleft*invz));
            cimg_forC(*this,c) {
              *ptrd = (T)(nbrightness**col);
              ptrd+=whd; col+=twh;
            }
            ptrd-=offx; zleft+=pentez; txleft+=pentetx; tyleft+=pentety;
          } else for (int x = xleft; x<=xright; ++x) {
            const float invz = 1/zleft;
            const tc *col = &texture._atXY((int)(txleft*invz),(int)(tyleft*invz));
            cimg_forC(*this,c) {
              *ptrd = (T)((2 - nbrightness)**col + (nbrightness - 1)*maxval);
              ptrd+=whd; col+=twh;
            }
            ptrd-=offx; zleft+=pentez; txleft+=pentetx; tyleft+=pentety;
          }
        } else {
          if (nbrightness==1) for (int x = xleft; x<=xright; ++x) {
            const float invz = 1/zleft;
            const tc *col = &texture._atXY((int)(txleft*invz),(int)(tyleft*invz));
            cimg_forC(*this,c) {
              *ptrd = (T)(nopacity**col + *ptrd*copacity);
              ptrd+=whd; col+=twh;
            }
            ptrd-=offx; zleft+=pentez; txleft+=pentetx; tyleft+=pentety;
          } else if (nbrightness<1) for (int x = xleft; x<=xright; ++x) {
            const float invz = 1/zleft;
            const tc *col = &texture._atXY((int)(txleft*invz),(int)(tyleft*invz));
            cimg_forC(*this,c) {
              *ptrd = (T)(nopacity*nbrightness**col + *ptrd*copacity);
              ptrd+=whd; col+=twh;
            }
            ptrd-=offx; zleft+=pentez; txleft+=pentetx; tyleft+=pentety;
          } else for (int x = xleft; x<=xright; ++x) {
            const float invz = 1/zleft;
            const tc *col = &texture._atXY((int)(txleft*invz),(int)(tyleft*invz));
            cimg_forC(*this,c) {
              const T val = (T)((2 - nbrightness)**col + (nbrightness - 1)*maxval);
              *ptrd = (T)(nopacity*val + *ptrd*copacity);
              ptrd+=whd; col+=twh;
            }
            ptrd-=offx; zleft+=pentez; txleft+=pentetx; tyleft+=pentety;
          }
        }
        zr+=pzr; txr+=ptxr; tyr+=ptyr; zl+=pzl; txl+=ptxl; tyl+=ptyl;
      }
      return *this;