    //! Rotate volumetric image with arbitrary angle and axis \newinstance.
    CImg<T> get_rotate(const float u, const float v, const float w, const float angle,
                       const unsigned int interpolation, const unsigned int boundary_conditions) const {
      if (is_empty()) return *this;
      CImg<T> res;
      const float
        w1 = _width - 1, h1 = _height - 1, d1 = _depth -1,
        w2 = 0.5f*w1, h2 = 0.5f*h1, d2 = 0.5f*d1;
      CImg<floatT> R = CImg<floatT>::rotation_matrix(u,v,w,angle);
      const CImg<Tfloat>
        X = R*CImg<Tfloat>(8,3,1,1,
                           0.0f,w1,w1,0.0f,0.0f,w1,w1,0.0f,
                           0.0f,0.0f,h1,h1,0.0f,0.0f,h1,h1,
                           0.0f,0.0f,0.0f,0.0f,d1,d1,d1,d1);
      float
        xm, xM = X.get_shared_row(0).max_min(xm),
        ym, yM = X.get_shared_row(1).max_min(ym),
        zm, zM = X.get_shared_row(2).max_min(zm);
      const int
        dx = (int)cimg::round(xM - xm),
        dy = (int)cimg::round(yM - ym),
        dz = (int)cimg::round(zM - zm);
      R.transpose();
      res.assign(1 + dx,1 + dy,1 + dz,_spectrum);
      const float rw2 = 0.5f*dx, rh2 = 0.5f*dy, rd2 = 0.5f*dz;
      _rotate(res,R,interpolation,boundary_conditions,w2,h2,d2,rw2,rh2,rd2);
      return res;