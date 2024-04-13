    **/
    CImg<T>& cumulate(const char axis=0) {
      switch (cimg::lowercase(axis)) {
      case 'x' :
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=512 && _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c) {
          T *ptrd = data(0,y,z,c);
          Tlong cumul = (Tlong)0;
          cimg_forX(*this,x) { cumul+=(Tlong)*ptrd; *(ptrd++) = (T)cumul; }
        }
        break;
      case 'y' : {
        const ulongT w = (ulongT)_width;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_height>=512 && _width*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c) {
          T *ptrd = data(x,0,z,c);
          Tlong cumul = (Tlong)0;
          cimg_forY(*this,y) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; ptrd+=w; }
        }
      } break;
      case 'z' : {
        const ulongT wh = (ulongT)_width*_height;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_depth>=512 && _width*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c) {
          T *ptrd = data(x,y,0,c);
          Tlong cumul = (Tlong)0;
          cimg_forZ(*this,z) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; ptrd+=wh; }
        }
      } break;
      case 'c' : {
        const ulongT whd = (ulongT)_width*_height*_depth;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_spectrum>=512 && _width*_height*_depth>=16))
        cimg_forXYZ(*this,x,y,z) {
          T *ptrd = data(x,y,z,0);
          Tlong cumul = (Tlong)0;
          cimg_forC(*this,c) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; ptrd+=whd; }
        }
      } break;
      default : { // Global cumulation.
        Tlong cumul = (Tlong)0;
        cimg_for(*this,ptrd,T) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; }
      }
      }
      return *this;