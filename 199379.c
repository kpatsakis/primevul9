    **/
    CImg<T>& normalize() {
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16))
      cimg_forYZ(*this,y,z) {
        T *ptrd = data(0,y,z,0);
        cimg_forX(*this,x) {
          const T *ptrs = ptrd;
          float n = 0;
          cimg_forC(*this,c) { n+=cimg::sqr((float)*ptrs); ptrs+=whd; }
          n = (float)std::sqrt(n);
          T *_ptrd = ptrd++;
          if (n>0) cimg_forC(*this,c) { *_ptrd = (T)(*_ptrd/n); _ptrd+=whd; }
          else cimg_forC(*this,c) { *_ptrd = (T)0; _ptrd+=whd; }
        }
      }
      return *this;