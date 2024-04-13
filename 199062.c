    **/
    CImgList<T> get_split(const char axis, const int nb=-1) const {
      CImgList<T> res;
      if (is_empty()) return res;
      const char _axis = cimg::lowercase(axis);

      if (nb<0) { // Split by bloc size.
        const unsigned int dp = (unsigned int)(nb?-nb:1);
        switch (_axis) {
        case 'x': {
          if (_width>dp) {
            res.assign(_width/dp + (_width%dp?1:0),1,1);
            const unsigned int pe = _width - dp;
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=128 && _height*_depth*_spectrum>=128))
            for (unsigned int p = 0; p<pe; p+=dp)
              get_crop(p,0,0,0,p + dp - 1,_height - 1,_depth - 1,_spectrum - 1).move_to(res[p/dp]);
            get_crop((res._width - 1)*dp,0,0,0,_width - 1,_height - 1,_depth - 1,_spectrum - 1).move_to(res.back());
          } else res.assign(*this);
        } break;
        case 'y': {
          if (_height>dp) {
            res.assign(_height/dp + (_height%dp?1:0),1,1);
            const unsigned int pe = _height - dp;
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=128 && _width*_depth*_spectrum>=128))
            for (unsigned int p = 0; p<pe; p+=dp)
              get_crop(0,p,0,0,_width - 1,p + dp - 1,_depth - 1,_spectrum - 1).move_to(res[p/dp]);
            get_crop(0,(res._width - 1)*dp,0,0,_width - 1,_height - 1,_depth - 1,_spectrum - 1).move_to(res.back());
          } else res.assign(*this);
        } break;
        case 'z': {
          if (_depth>dp) {
            res.assign(_depth/dp + (_depth%dp?1:0),1,1);
            const unsigned int pe = _depth - dp;
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=128 && _width*_height*_spectrum>=128))
            for (unsigned int p = 0; p<pe; p+=dp)
              get_crop(0,0,p,0,_width - 1,_height - 1,p + dp - 1,_spectrum - 1).move_to(res[p/dp]);
            get_crop(0,0,(res._width - 1)*dp,0,_width - 1,_height - 1,_depth - 1,_spectrum - 1).move_to(res.back());
          } else res.assign(*this);
        } break;
        case 'c' : {
          if (_spectrum>dp) {
            res.assign(_spectrum/dp + (_spectrum%dp?1:0),1,1);
            const unsigned int pe = _spectrum - dp;
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=128 && _width*_height*_depth>=128))
            for (unsigned int p = 0; p<pe; p+=dp)
              get_crop(0,0,0,p,_width - 1,_height - 1,_depth - 1,p + dp - 1).move_to(res[p/dp]);
            get_crop(0,0,0,(res._width - 1)*dp,_width - 1,_height - 1,_depth - 1,_spectrum - 1).move_to(res.back());
          } else res.assign(*this);
        }
        }
      } else if (nb>0) { // Split by number of (non-homogeneous) blocs.
        const unsigned int siz = _axis=='x'?_width:_axis=='y'?_height:_axis=='z'?_depth:_axis=='c'?_spectrum:0;
        if ((unsigned int)nb>siz)
          throw CImgArgumentException(_cimg_instance
                                      "get_split(): Instance cannot be split along %c-axis into %u blocs.",
                                      cimg_instance,
                                      axis,nb);
        if (nb==1) res.assign(*this);
        else {
          int err = (int)siz;
          unsigned int _p = 0;
          switch (_axis) {
          case 'x' : {
            cimg_forX(*this,p) if ((err-=nb)<=0) {
              get_crop(_p,0,0,0,p,_height - 1,_depth - 1,_spectrum - 1).move_to(res);
              err+=(int)siz;
              _p = p + 1U;
            }
          } break;
          case 'y' : {
            cimg_forY(*this,p) if ((err-=nb)<=0) {
              get_crop(0,_p,0,0,_width - 1,p,_depth - 1,_spectrum - 1).move_to(res);
              err+=(int)siz;
              _p = p + 1U;
            }
          } break;
          case 'z' : {
            cimg_forZ(*this,p) if ((err-=nb)<=0) {
              get_crop(0,0,_p,0,_width - 1,_height - 1,p,_spectrum - 1).move_to(res);
              err+=(int)siz;
              _p = p + 1U;
            }
          } break;
          case 'c' : {
            cimg_forC(*this,p) if ((err-=nb)<=0) {
              get_crop(0,0,0,_p,_width - 1,_height - 1,_depth - 1,p).move_to(res);
              err+=(int)siz;
              _p = p + 1U;
            }
          }
          }
        }
      } else { // Split by egal values according to specified axis.
        T current = *_data;
        switch (_axis) {
        case 'x' : {
          int i0 = 0;
          cimg_forX(*this,i)
            if ((*this)(i)!=current) { get_columns(i0,i - 1).move_to(res); i0 = i; current = (*this)(i); }
          get_columns(i0,width() - 1).move_to(res);
        } break;
        case 'y' : {
          int i0 = 0;
          cimg_forY(*this,i)
            if ((*this)(0,i)!=current) { get_rows(i0,i - 1).move_to(res); i0 = i; current = (*this)(0,i); }
          get_rows(i0,height() - 1).move_to(res);
        } break;
        case 'z' : {
          int i0 = 0;
          cimg_forZ(*this,i)
            if ((*this)(0,0,i)!=current) { get_slices(i0,i - 1).move_to(res); i0 = i; current = (*this)(0,0,i); }
          get_slices(i0,depth() - 1).move_to(res);
        } break;
        case 'c' : {
          int i0 = 0;
          cimg_forC(*this,i)
            if ((*this)(0,0,0,i)!=current) { get_channels(i0,i - 1).move_to(res); i0 = i; current = (*this)(0,0,0,i); }
          get_channels(i0,spectrum() - 1).move_to(res);
        } break;
        default : {
          longT i0 = 0;
          cimg_foroff(*this,i)
            if ((*this)[i]!=current) {
              CImg<T>(_data + i0,1,(unsigned int)(i - i0)).move_to(res);
              i0 = (longT)i; current = (*this)[i];
            }
          CImg<T>(_data + i0,1,(unsigned int)(size() - i0)).move_to(res);
        }
        }
      }
      return res;