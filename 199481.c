    template<typename tc>
    CImg<T>& draw_line(const int x0, const int y0,
                       const int x1, const int y1,
                       const CImg<tc>& texture,
                       const int tx0, const int ty0,
                       const int tx1, const int ty1,
                       const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture)) return draw_line(x0,y0,x1,y1,+texture,tx0,ty0,tx1,ty1,opacity,pattern,init_hatch);
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      const bool xdir = x0<x1, ydir = y0<y1;
      int
        dtx = tx1-tx0, dty = ty1-ty0,
        nx0 = x0, nx1 = x1, ny0 = y0, ny1 = y1,
        tnx0 = tx0, tnx1 = tx1, tny0 = ty0, tny1 = ty1,
        &xleft = xdir?nx0:nx1, &yleft = xdir?ny0:ny1, &xright = xdir?nx1:nx0, &yright = xdir?ny1:ny0,
        &txleft = xdir?tnx0:tnx1, &tyleft = xdir?tny0:tny1, &txright = xdir?tnx1:tnx0, &tyright = xdir?tny1:tny0,
        &xup = ydir?nx0:nx1, &yup = ydir?ny0:ny1, &xdown = ydir?nx1:nx0, &ydown = ydir?ny1:ny0,
        &txup = ydir?tnx0:tnx1, &tyup = ydir?tny0:tny1, &txdown = ydir?tnx1:tnx0, &tydown = ydir?tny1:tny0;
      if (xright<0 || xleft>=width()) return *this;
      if (xleft<0) {
        const float D = (float)xright - xleft;
        yleft-=(int)((float)xleft*((float)yright - yleft)/D);
        txleft-=(int)((float)xleft*((float)txright - txleft)/D);
        tyleft-=(int)((float)xleft*((float)tyright - tyleft)/D);
        xleft = 0;
      }
      if (xright>=width()) {
        const float d = (float)xright - width(), D = (float)xright - xleft;
        yright-=(int)(d*((float)yright - yleft)/D);
        txright-=(int)(d*((float)txright - txleft)/D);
        tyright-=(int)(d*((float)tyright - tyleft)/D);
        xright = width() - 1;
      }
      if (ydown<0 || yup>=height()) return *this;
      if (yup<0) {
        const float D = (float)ydown - yup;
        xup-=(int)((float)yup*((float)xdown - xup)/D);
        txup-=(int)((float)yup*((float)txdown - txup)/D);
        tyup-=(int)((float)yup*((float)tydown - tyup)/D);
        yup = 0;
      }
      if (ydown>=height()) {
        const float d = (float)ydown - height(), D = (float)ydown - yup;
        xdown-=(int)(d*((float)xdown - xup)/D);
        txdown-=(int)(d*((float)txdown - txup)/D);
        tydown-=(int)(d*((float)tydown - tyup)/D);
        ydown = height() - 1;
      }
      T *ptrd0 = data(nx0,ny0);
      int dx = xright - xleft, dy = ydown - yup;
      const bool steep = dy>dx;
      if (steep) cimg::swap(nx0,ny0,nx1,ny1,dx,dy);
      const longT
        offx = (longT)(nx0<nx1?1:-1)*(steep?width():1),
        offy = (longT)(ny0<ny1?1:-1)*(steep?1:width()),
        ndx = (longT)(dx>0?dx:1);
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        twh = (ulongT)texture._width*texture._height;

      if (opacity>=1) {
        if (~pattern) for (int error = dx>>1, x = 0; x<=dx; ++x) {
          if (pattern&hatch) {
            T *ptrd = ptrd0;
            const int tx = tx0 + x*dtx/ndx, ty = ty0 + x*dty/ndx;
            const tc *col = &texture._atXY(tx,ty);
            cimg_forC(*this,c) { *ptrd = (T)*col; ptrd+=whd; col+=twh; }
          }
          hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1);
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        } else for (int error = dx>>1, x = 0; x<=dx; ++x) {
          T *ptrd = ptrd0;
          const int tx = tx0 + x*dtx/ndx, ty = ty0 + x*dty/ndx;
          const tc *col = &texture._atXY(tx,ty);
          cimg_forC(*this,c) { *ptrd = (T)*col; ptrd+=whd; col+=twh; }
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        }
      } else {
        const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
        if (~pattern) for (int error = dx>>1, x = 0; x<=dx; ++x) {
          T *ptrd = ptrd0;
          if (pattern&hatch) {
            const int tx = tx0 + x*dtx/ndx, ty = ty0 + x*dty/ndx;
            const tc *col = &texture._atXY(tx,ty);
            cimg_forC(*this,c) { *ptrd = (T)(nopacity**col + *ptrd*copacity); ptrd+=whd; col+=twh; }
          }
          hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1);
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        } else for (int error = dx>>1, x = 0; x<=dx; ++x) {
          T *ptrd = ptrd0;
          const int tx = tx0 + x*dtx/ndx, ty = ty0 + x*dty/ndx;
          const tc *col = &texture._atXY(tx,ty);
          cimg_forC(*this,c) { *ptrd = (T)(nopacity**col + *ptrd*copacity); ptrd+=whd; col+=twh; }
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        }
      }
      return *this;