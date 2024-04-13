    // [internal] Perform 3d rotation with arbitrary axis and angle.
    void _rotate(CImg<T>& res, const CImg<Tfloat>& R,
                 const unsigned int interpolation, const unsigned int boundary_conditions,
                 const float w2, const float h2, const float d2,
                 const float rw2, const float rh2, const float rd2) const {
      switch (boundary_conditions) {
      case 3 : // Mirror
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          const float ww = 2.0f*width(), hh = 2.0f*height(), dd = 2.0f*depth();
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = cimg::mod((float)(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),ww),
              Y = cimg::mod((float)(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),hh),
              Z = cimg::mod((float)(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),dd);
            cimg_forC(res,c) res(x,y,z,c) = _cubic_cut_atXYZ(X<width()?X:ww - X - 1,
                                                             Y<height()?Y:hh - Y - 1,
                                                             Z<depth()?Z:dd - Z - z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          const float ww = 2.0f*width(), hh = 2.0f*height(), dd = 2.0f*depth();
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = cimg::mod((float)(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),ww),
              Y = cimg::mod((float)(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),hh),
              Z = cimg::mod((float)(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),dd);
            cimg_forC(res,c) res(x,y,z,c) = (T)_linear_atXYZ(X<width()?X:ww - X - 1,
                                                             Y<height()?Y:hh - Y - 1,
                                                             Z<depth()?Z:dd - Z - 1,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          const int ww = 2*width(), hh = 2*height(), dd = 2*depth();
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = cimg::mod((int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),ww),
              Y = cimg::mod((int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),hh),
              Z = cimg::mod((int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),dd);
            cimg_forC(res,c) res(x,y,z,c) = (*this)(X<width()?X:ww - X - 1,
                                                    Y<height()?Y:hh - Y - 1,
                                                    Z<depth()?Z:dd - Z -  1,c);
          }
        }
        } break;

      case 2 : // Periodic
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = cimg::mod((float)(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),(float)width()),
              Y = cimg::mod((float)(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),(float)height()),
              Z = cimg::mod((float)(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),(float)depth());
            cimg_forC(res,c) res(x,y,z,c) = _cubic_cut_atXYZ(X,Y,Z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = cimg::mod((float)(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),(float)width()),
              Y = cimg::mod((float)(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),(float)height()),
              Z = cimg::mod((float)(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),(float)depth());
            cimg_forC(res,c) res(x,y,z,c) = (T)_linear_atXYZ(X,Y,Z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = cimg::mod((int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),width()),
              Y = cimg::mod((int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),height()),
              Z = cimg::mod((int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),depth());
            cimg_forC(res,c) res(x,y,z,c) = (*this)(X,Y,Z,c);
          }
        }
        } break;

      case 1 : // Neumann
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = _cubic_cut_atXYZ(X,Y,Z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = _linear_atXYZ(X,Y,Z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = (int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),
              Y = (int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),
              Z = (int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc);
            cimg_forC(res,c) res(x,y,z,c) = _atXYZ(X,Y,Z,c);
          }
        }
        } break;

      default : // Dirichlet
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = cubic_cut_atXYZ(X,Y,Z,c,(T)0);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = linear_atXYZ(X,Y,Z,c,(T)0);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = (int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),
              Y = (int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),
              Z = (int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc);
            cimg_forC(res,c) res(x,y,z,c) = atXYZ(X,Y,Z,c,(T)0);
          }
        }
        } break;
      }