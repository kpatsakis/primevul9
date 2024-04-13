    template<typename t>
    CImg<T>& cross(const CImg<t>& img) {
      if (_width!=1 || _height<3 || img._width!=1 || img._height<3)
        throw CImgInstanceException(_cimg_instance
                                    "cross(): Instance and/or specified image (%u,%u,%u,%u,%p) are not 3d vectors.",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);

      const T x = (*this)[0], y = (*this)[1], z = (*this)[2];
      (*this)[0] = (T)(y*img[2] - z*img[1]);
      (*this)[1] = (T)(z*img[0] - x*img[2]);
      (*this)[2] = (T)(x*img[1] - y*img[0]);
      return *this;