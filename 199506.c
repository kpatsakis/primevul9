    template<typename tf, typename tc, typename te>
    CImg<floatT> get_elevation3d(CImgList<tf>& primitives, CImgList<tc>& colors, const CImg<te>& elevation) const {
      if (!is_sameXY(elevation) || elevation._depth>1 || elevation._spectrum>1)
        throw CImgArgumentException(_cimg_instance
                                    "get_elevation3d(): Instance and specified elevation (%u,%u,%u,%u,%p) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    elevation._width,elevation._height,elevation._depth,
                                    elevation._spectrum,elevation._data);
      if (is_empty()) return *this;
      float m, M = (float)max_min(m);
      if (M==m) ++M;
      colors.assign();
      const unsigned int size_x1 = _width - 1, size_y1 = _height - 1;
      for (unsigned int y = 0; y<size_y1; ++y)
        for (unsigned int x = 0; x<size_x1; ++x) {
          const unsigned char
            r = (unsigned char)(((*this)(x,y,0) - m)*255/(M-m)),
            g = (unsigned char)(_spectrum>1?((*this)(x,y,1) - m)*255/(M-m):r),
            b = (unsigned char)(_spectrum>2?((*this)(x,y,2) - m)*255/(M-m):_spectrum>1?0:r);
          CImg<tc>::vector((tc)r,(tc)g,(tc)b).move_to(colors);
        }
      const typename CImg<te>::_functor2d_int func(elevation);
      return elevation3d(primitives,func,0,0,_width - 1.0f,_height - 1.0f,_width,_height);