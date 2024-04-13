    //! Resize 3d object to unit size.
    CImg<T> resize_object3d() {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "resize_object3d(): Instance is not a set of 3d vertices.",
                                    cimg_instance);

      CImg<T> xcoords = get_shared_row(0), ycoords = get_shared_row(1), zcoords = get_shared_row(2);
      float
        xm, xM = (float)xcoords.max_min(xm),
        ym, yM = (float)ycoords.max_min(ym),
        zm, zM = (float)zcoords.max_min(zm);
      const float dx = xM - xm, dy = yM - ym, dz = zM - zm, dmax = cimg::max(dx,dy,dz);
      if (dmax>0) { xcoords/=dmax; ycoords/=dmax; zcoords/=dmax; }
      return *this;