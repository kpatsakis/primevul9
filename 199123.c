    template<typename t>
    CImg<_cimg_Tt> operator*(const CImg<t>& img) const {
      if (_width!=img._height || _depth!=1 || _spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "operator*(): Invalid multiplication of instance by specified "
                                    "matrix (%u,%u,%u,%u,%p)",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);
      CImg<_cimg_Tt> res(img._width,_height);
#ifdef cimg_use_openmp
      cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(size()>1024 && img.size()>1024))
      cimg_forXY(res,i,j) {
        _cimg_Ttdouble value = 0; cimg_forX(*this,k) value+=(*this)(k,j)*img(i,k); res(i,j) = (_cimg_Tt)value;
      }
#else
      _cimg_Tt *ptrd = res._data;
      cimg_forXY(res,i,j) {
        _cimg_Ttdouble value = 0; cimg_forX(*this,k) value+=(*this)(k,j)*img(i,k); *(ptrd++) = (_cimg_Tt)value;
      }
#endif
      return res;