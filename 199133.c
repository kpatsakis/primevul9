    template<typename tc>
    CImg<T>& draw_mandelbrot(const int x0, const int y0, const int x1, const int y1,
                             const CImg<tc>& colormap, const float opacity=1,
                             const double z0r=-2, const double z0i=-2, const double z1r=2, const double z1i=2,
                             const unsigned int iteration_max=255,
                             const bool is_normalized_iteration=false,
                             const bool is_julia_set=false,
                             const double param_r=0, const double param_i=0) {
      if (is_empty()) return *this;
      CImg<tc> palette;
      if (colormap) palette.assign(colormap._data,colormap.size()/colormap._spectrum,1,1,colormap._spectrum,true);
      if (palette && palette._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_mandelbrot(): Instance and specified colormap (%u,%u,%u,%u,%p) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    colormap._width,colormap._height,colormap._depth,colormap._spectrum,colormap._data);

      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f), ln2 = (float)std::log(2.0);
      const int
        _x0 = cimg::cut(x0,0,width() - 1),
        _y0 = cimg::cut(y0,0,height() - 1),
        _x1 = cimg::cut(x1,0,width() - 1),
        _y1 = cimg::cut(y1,0,height() - 1);

      cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if((1 + _x1 - _x0)*(1 + _y1 - _y0)>=2048))
      for (int q = _y0; q<=_y1; ++q)
        for (int p = _x0; p<=_x1; ++p) {
          unsigned int iteration = 0;
          const double x = z0r + p*(z1r-z0r)/_width, y = z0i + q*(z1i-z0i)/_height;
          double zr, zi, cr, ci;
          if (is_julia_set) { zr = x; zi = y; cr = param_r; ci = param_i; }
          else { zr = param_r; zi = param_i; cr = x; ci = y; }
          for (iteration=1; zr*zr + zi*zi<=4 && iteration<=iteration_max; ++iteration) {
            const double temp = zr*zr - zi*zi + cr;
            zi = 2*zr*zi + ci;
            zr = temp;
          }
          if (iteration>iteration_max) {
            if (palette) {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)palette(0,c);
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(palette(0,c)*nopacity + (*this)(p,q,0,c)*copacity);
            } else {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)0;
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)((*this)(p,q,0,c)*copacity);
            }
          } else if (is_normalized_iteration) {
            const float
              normz = (float)cimg::abs(zr*zr + zi*zi),
              niteration = (float)(iteration + 1 - std::log(std::log(normz))/ln2);
            if (palette) {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)palette._linear_atX(niteration,c);
              else cimg_forC(*this,c)
                     (*this)(p,q,0,c) = (T)(palette._linear_atX(niteration,c)*nopacity + (*this)(p,q,0,c)*copacity);
            } else {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)niteration;
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(niteration*nopacity + (*this)(p,q,0,c)*copacity);
            }
          } else {
            if (palette) {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)palette._atX(iteration,c);
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(palette(iteration,c)*nopacity + (*this)(p,q,0,c)*copacity);
            } else {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)iteration;
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(iteration*nopacity + (*this)(p,q,0,c)*copacity);
            }
          }
        }
      return *this;