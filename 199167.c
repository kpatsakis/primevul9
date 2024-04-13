    template<typename tc>
    CImg<T>& draw_line(const int x0, const int y0,
                       const int x1, const int y1,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Specified color is (null).",
                                    cimg_instance);
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      const bool xdir = x0<x1, ydir = y0<y1;
      int
	nx0 = x0, nx1 = x1, ny0 = y0, ny1 = y1,
	&xleft = xdir?nx0:nx1, &yleft = xdir?ny0:ny1,
        &xright = xdir?nx1:nx0, &yright = xdir?ny1:ny0,
	&xup = ydir?nx0:nx1, &yup = ydir?ny0:ny1,
        &xdown = ydir?nx1:nx0, &ydown = ydir?ny1:ny0;
      if (xright<0 || xleft>=width()) return *this;
      if (xleft<0) { yleft-=(int)((float)xleft*((float)yright - yleft)/((float)xright - xleft)); xleft = 0; }
      if (xright>=width()) {
        yright-=(int)(((float)xright - width())*((float)yright - yleft)/((float)xright - xleft));
        xright = width() - 1;
      }
      if (ydown<0 || yup>=height()) return *this;
      if (yup<0) { xup-=(int)((float)yup*((float)xdown - xup)/((float)ydown - yup)); yup = 0; }
      if (ydown>=height()) {
        xdown-=(int)(((float)ydown - height())*((float)xdown - xup)/((float)ydown - yup));
        ydown = height() - 1;
      }
      T *ptrd0 = data(nx0,ny0);
      int dx = xright - xleft, dy = ydown - yup;
      const bool steep = dy>dx;
      if (steep) cimg::swap(nx0,ny0,nx1,ny1,dx,dy);
      const longT
        offx = (longT)(nx0<nx1?1:-1)*(steep?width():1),
        offy = (longT)(ny0<ny1?1:-1)*(steep?1:width());
      const ulongT wh = (ulongT)_width*_height;
      if (opacity>=1) {
        if (~pattern) for (int error = dx>>1, x = 0; x<=dx; ++x) {
          if (pattern&hatch) {
            T *ptrd = ptrd0; const tc* col = color;
            cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=wh; }
          }
          hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1);
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        } else for (int error = dx>>1, x = 0; x<=dx; ++x) {
          T *ptrd = ptrd0; const tc* col = color; cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=wh; }
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        }
      } else {
        const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
        if (~pattern) for (int error = dx>>1, x = 0; x<=dx; ++x) {
          if (pattern&hatch) {
            T *ptrd = ptrd0; const tc* col = color;
            cimg_forC(*this,c) { *ptrd = (T)(nopacity**(col++) + *ptrd*copacity); ptrd+=wh; }
          }
          hatch>>=1; if (!hatch) hatch = ~0U - (~0U>>1);
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        } else for (int error = dx>>1, x = 0; x<=dx; ++x) {
          T *ptrd = ptrd0; const tc* col = color;
          cimg_forC(*this,c) { *ptrd = (T)(nopacity**(col++) + *ptrd*copacity); ptrd+=wh; }
          ptrd0+=offx;
          if ((error-=dy)<0) { ptrd0+=offy; error+=dx; }
        }
      }
      return *this;