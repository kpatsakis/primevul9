    **/
    CImg<T>& resize_object3d(const float sx, const float sy=-100, const float sz=-100) {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "resize_object3d(): Instance is not a set of 3d vertices.",
                                    cimg_instance);

      CImg<T> xcoords = get_shared_row(0), ycoords = get_shared_row(1), zcoords = get_shared_row(2);
      float
        xm, xM = (float)xcoords.max_min(xm),
        ym, yM = (float)ycoords.max_min(ym),
        zm, zM = (float)zcoords.max_min(zm);
      if (xm<xM) { if (sx>0) xcoords*=sx/(xM-xm); else xcoords*=-sx/100; }
      if (ym<yM) { if (sy>0) ycoords*=sy/(yM-ym); else ycoords*=-sy/100; }
      if (zm<zM) { if (sz>0) zcoords*=sz/(zM-zm); else zcoords*=-sz/100; }
      return *this;