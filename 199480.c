    **/
    CImg<T>& boxfilter(const float boxsize, const int order, const char axis='x',
                       const bool boundary_conditions=true,
                       const unsigned int nb_iter=1) {
      if (is_empty() || !boxsize || (boxsize<=1 && !order)) return *this;
      const char naxis = cimg::lowercase(axis);
      const float nboxsize = boxsize>=0?boxsize:-boxsize*
        (naxis=='x'?_width:naxis=='y'?_height:naxis=='z'?_depth:_spectrum)/100;
      switch (naxis) {
      case 'x' : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c)
          _cimg_blur_box_apply(data(0,y,z,c),nboxsize,_width,1U,order,boundary_conditions,nb_iter);
      } break;
      case 'y' : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c)
          _cimg_blur_box_apply(data(x,0,z,c),nboxsize,_height,(ulongT)_width,order,boundary_conditions,nb_iter);
      } break;
      case 'z' : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c)
          _cimg_blur_box_apply(data(x,y,0,c),nboxsize,_depth,(ulongT)_width*_height,order,boundary_conditions,nb_iter);
      } break;
      default : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forXYZ(*this,x,y,z)
          _cimg_blur_box_apply(data(x,y,z,0),nboxsize,_spectrum,(ulongT)_width*_height*_depth,
                               order,boundary_conditions,nb_iter);
      }
      }
      return *this;