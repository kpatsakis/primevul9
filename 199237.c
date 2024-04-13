    // [internal] Perform 2d rotation with arbitrary angle.
    void _rotate(CImg<T>& res, const float angle,
                 const unsigned int interpolation, const unsigned int boundary_conditions,
                 const float w2, const float h2,
                 const float rw2, const float rh2) const {
      const float
        rad = (float)(angle*cimg::PI/180.0),
        ca = (float)std::cos(rad), sa = (float)std::sin(rad);

      switch (boundary_conditions) {
      case 3 : { // Mirror

        switch (interpolation) {
        case 2 : { // Cubic interpolation
          const float ww = 2.0f*width(), hh = 2.0f*height();
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
            cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2,
              mx = cimg::mod(w2 + xc*ca + yc*sa,ww),
              my = cimg::mod(h2 - xc*sa + yc*ca,hh);
            res(x,y,z,c) = _cubic_cut_atXY(mx<width()?mx:ww - mx - 1,my<height()?my:hh - my - 1,z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          const float ww = 2.0f*width(), hh = 2.0f*height();
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
            cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2,
              mx = cimg::mod(w2 + xc*ca + yc*sa,ww),
              my = cimg::mod(h2 - xc*sa + yc*ca,hh);
            res(x,y,z,c) = (T)_linear_atXY(mx<width()?mx:ww - mx - 1,my<height()?my:hh - my - 1,z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          const int ww = 2*width(), hh = 2*height();
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
            cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2,
              mx = cimg::mod((int)cimg::round(w2 + xc*ca + yc*sa),ww),
              my = cimg::mod((int)cimg::round(h2 - xc*sa + yc*ca),hh);
            res(x,y,z,c) = (*this)(mx<width()?mx:ww - mx - 1,my<height()?my:hh - my - 1,z,c);
          }
        }
        }
      } break;

      case 2 : // Periodic
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
            cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = _cubic_cut_atXY(cimg::mod(w2 + xc*ca + yc*sa,(float)width()),
                                           cimg::mod(h2 - xc*sa + yc*ca,(float)height()),z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
            cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (T)_linear_atXY(cimg::mod(w2 + xc*ca + yc*sa,(float)width()),
                                           cimg::mod(h2 - xc*sa + yc*ca,(float)height()),z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
            cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (*this)(cimg::mod((int)cimg::round(w2 + xc*ca + yc*sa),(float)width()),
                                   cimg::mod((int)cimg::round(h2 - xc*sa + yc*ca),(float)height()),z,c);
          }
        }
        } break;

      case 1 : // Neumann
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = _cubic_cut_atXY(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (T)_linear_atXY(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = _atXY((int)cimg::round(w2 + xc*ca + yc*sa),
                                 (int)cimg::round(h2 - xc*sa + yc*ca),z,c);
          }
        }
        } break;

      default : // Dirichlet
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = cubic_cut_atXY(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c,(T)0);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (T)linear_atXY(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c,(T)0);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = atXY((int)cimg::round(w2 + xc*ca + yc*sa),
                                (int)cimg::round(h2 - xc*sa + yc*ca),z,c,(T)0);
          }
        }
        }
      }