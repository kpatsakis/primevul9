    **/
    double variance_noise(const unsigned int variance_method=2) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "variance_noise(): Empty instance.",
                                    cimg_instance);

      const ulongT siz = size();
      if (!siz || !_data) return 0;
      if (variance_method>1) { // Compute a scaled version of the Laplacian.
        CImg<Tdouble> tmp(*this,false);
        if (_depth==1) {
          const double cste = 1.0/std::sqrt(20.0); // Depends on how the Laplacian is computed.
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=262144 && _spectrum>=2))
          cimg_forC(*this,c) {
            CImg_3x3(I,T);
            cimg_for3x3(*this,x,y,0,c,I,T) {
              tmp(x,y,c) = cste*((double)Inc + (double)Ipc + (double)Icn +
                                 (double)Icp - 4*(double)Icc);
            }
          }
        } else {
          const double cste = 1.0/std::sqrt(42.0); // Depends on how the Laplacian is computed.
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=262144 && _spectrum>=2))
          cimg_forC(*this,c) {
            CImg_3x3x3(I,T);
            cimg_for3x3x3(*this,x,y,z,c,I,T) {
              tmp(x,y,z,c) = cste*(
                                   (double)Incc + (double)Ipcc + (double)Icnc + (double)Icpc +
                                   (double)Iccn + (double)Iccp - 6*(double)Iccc);
            }
          }
        }
        return tmp.variance(variance_method);
      }

      // Version that doesn't need intermediate images.
      double variance = 0, S = 0, S2 = 0;
      if (_depth==1) {
        const double cste = 1.0/std::sqrt(20.0);
        CImg_3x3(I,T);
        cimg_forC(*this,c) cimg_for3x3(*this,x,y,0,c,I,T) {
          const double val = cste*((double)Inc + (double)Ipc +
                                   (double)Icn + (double)Icp - 4*(double)Icc);
          S+=val; S2+=val*val;
        }
      } else {
        const double cste = 1.0/std::sqrt(42.0);
        CImg_3x3x3(I,T);
        cimg_forC(*this,c) cimg_for3x3x3(*this,x,y,z,c,I,T) {
          const double val = cste *
            ((double)Incc + (double)Ipcc + (double)Icnc +
             (double)Icpc +
             (double)Iccn + (double)Iccp - 6*(double)Iccc);
          S+=val; S2+=val*val;
        }
      }
      if (variance_method) variance = siz>1?(S2 - S*S/siz)/(siz - 1):0;
      else variance = (S2 - S*S/siz)/siz;
      return variance>0?variance:0;