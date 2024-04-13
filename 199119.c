    template<typename t>
    const CImg<T>& eigen(CImg<t>& val, CImg<t> &vec) const {
      if (is_empty()) { val.assign(); vec.assign(); }
      else {
        if (_width!=_height || _depth>1 || _spectrum>1)
          throw CImgInstanceException(_cimg_instance
                                      "eigen(): Instance is not a square matrix.",
                                      cimg_instance);

        if (val.size()<(ulongT)_width) val.assign(1,_width);
        if (vec.size()<(ulongT)_width*_width) vec.assign(_width,_width);
        switch (_width) {
        case 1 : { val[0] = (t)(*this)[0]; vec[0] = (t)1; } break;
        case 2 : {
          const double a = (*this)[0], b = (*this)[1], c = (*this)[2], d = (*this)[3], e = a + d;
          double f = e*e - 4*(a*d - b*c);
          if (f<0)
            cimg::warn(_cimg_instance
                       "eigen(): Complex eigenvalues found.",
                       cimg_instance);

          f = std::sqrt(f);
          const double
            l1 = 0.5*(e - f),
            l2 = 0.5*(e + f),
            b2 = b*b,
            norm1 = std::sqrt(cimg::sqr(l2 - a) + b2),
            norm2 = std::sqrt(cimg::sqr(l1 - a) + b2);
          val[0] = (t)l2;
          val[1] = (t)l1;
          if (norm1>0) { vec(0,0) = (t)(b/norm1); vec(0,1) = (t)((l2 - a)/norm1); } else { vec(0,0) = 1; vec(0,1) = 0; }
          if (norm2>0) { vec(1,0) = (t)(b/norm2); vec(1,1) = (t)((l1 - a)/norm2); } else { vec(1,0) = 1; vec(1,1) = 0; }
        } break;
        default :
          throw CImgInstanceException(_cimg_instance
                                      "eigen(): Eigenvalues computation of general matrices is limited "
                                      "to 2x2 matrices.",
                                      cimg_instance);
        }
      }
      return *this;