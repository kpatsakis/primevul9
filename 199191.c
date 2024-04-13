    template<typename t, typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const float zc, const CImg<t>& tensor,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      typedef typename CImg<t>::Tfloat tfloat;
      if (tensor._width!=3 || tensor._height!=3 || tensor._depth!=1 || tensor._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified tensor (%u,%u,%u,%u,%p) is not a 3x3 matrix.",
                                    cimg_instance,
                                    tensor._width,tensor._height,tensor._depth,tensor._spectrum,tensor._data);

      const CImg<tfloat> invT = tensor.get_invert(), invT2 = (invT*invT)/(-2.0);
      const tfloat a = invT2(0,0), b = 2*invT2(1,0), c = 2*invT2(2,0), d = invT2(1,1), e = 2*invT2(2,1), f = invT2(2,2);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const tc *col = color;
      cimg_forXYZ(*this,x,y,z) {
        const float
          dx = (x - xc), dy = (y - yc), dz = (z - zc),
          val = (float)std::exp(a*dx*dx + b*dx*dy + c*dx*dz + d*dy*dy + e*dy*dz + f*dz*dz);
        T *ptrd = data(x,y,z,0);
        if (opacity>=1) cimg_forC(*this,c) { *ptrd = (T)(val*(*col++)); ptrd+=whd; }
        else cimg_forC(*this,c) { *ptrd = (T)(nopacity*val*(*col++) + *ptrd*copacity); ptrd+=whd; }
        col-=_spectrum;
      }
      return *this;