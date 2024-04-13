    template<typename tf>
    CImg<floatT> get_isoline3d(CImgList<tf>& primitives, const float isovalue,
                               const int size_x=-100, const int size_y=-100) const {
      if (_spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "get_isoline3d(): Instance is not a scalar image.",
                                    cimg_instance);
      if (_depth>1)
        throw CImgInstanceException(_cimg_instance
                                    "get_isoline3d(): Instance is not a 2d image.",
                                    cimg_instance);
      primitives.assign();
      if (is_empty()) return *this;
      CImg<floatT> vertices;
      if ((size_x==-100 && size_y==-100) || (size_x==width() && size_y==height())) {
        const _functor2d_int func(*this);
        vertices = isoline3d(primitives,func,isovalue,0,0,width() - 1.0f,height() - 1.0f,width(),height());
      } else {
        const _functor2d_float func(*this);
        vertices = isoline3d(primitives,func,isovalue,0,0,width() - 1.0f,height() - 1.0f,size_x,size_y);
      }
      return vertices;