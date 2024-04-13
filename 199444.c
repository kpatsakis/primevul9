    **/
    CImg<T>& shift_object3d(const float tx, const float ty=0, const float tz=0) {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "shift_object3d(): Instance is not a set of 3d vertices.",
                                    cimg_instance);

      get_shared_row(0)+=tx; get_shared_row(1)+=ty; get_shared_row(2)+=tz;
      return *this;