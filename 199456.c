    **/
    CImg<T>& shift_object3d() {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "shift_object3d(): Instance is not a set of 3d vertices.",
                                    cimg_instance);

      CImg<T> xcoords = get_shared_row(0), ycoords = get_shared_row(1), zcoords = get_shared_row(2);
      float
        xm, xM = (float)xcoords.max_min(xm),
        ym, yM = (float)ycoords.max_min(ym),
        zm, zM = (float)zcoords.max_min(zm);
      xcoords-=(xm + xM)/2; ycoords-=(ym + yM)/2; zcoords-=(zm + zM)/2;
      return *this;