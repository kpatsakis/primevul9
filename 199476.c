    //! Compute L2-norm of each multi-valued pixel of the image instance \newinstance.
    CImg<Tfloat> get_norm(const int norm_type=2) const {
      if (is_empty()) return *this;
      if (_spectrum==1 && norm_type) return get_abs();
      const ulongT whd = (ulongT)_width*_height*_depth;
      CImg<Tfloat> res(_width,_height,_depth);
      switch (norm_type) {
      case -1 : { // Linf-norm.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { const Tfloat val = (Tfloat)cimg::abs(*_ptrs); if (val>n) n = val; _ptrs+=whd; }
            *(ptrd++) = n;
          }
        }
      } break;
      case 0 : { // L0-norm.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            unsigned int n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=*_ptrs==0?0:1; _ptrs+=whd; }
            *(ptrd++) = (Tfloat)n;
          }
        }
      } break;
      case 1 : { // L1-norm.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=cimg::abs(*_ptrs); _ptrs+=whd; }
            *(ptrd++) = n;
          }
        }
      } break;
      case 2 : { // L2-norm.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=cimg::sqr((Tfloat)*_ptrs); _ptrs+=whd; }
            *(ptrd++) = (Tfloat)std::sqrt((Tfloat)n);
          }
        }
      } break;
      default : { // Linf-norm.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=std::pow(cimg::abs((Tfloat)*_ptrs),(Tfloat)norm_type); _ptrs+=whd; }
            *(ptrd++) = (Tfloat)std::pow((Tfloat)n,1/(Tfloat)norm_type);
          }
        }
      }
      }
      return res;