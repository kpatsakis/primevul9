    **/
    CImg<T>& noise(const double sigma, const unsigned int noise_type=0) {
      if (is_empty()) return *this;
      const Tfloat vmin = (Tfloat)cimg::type<T>::min(), vmax = (Tfloat)cimg::type<T>::max();
      Tfloat nsigma = (Tfloat)sigma, m = 0, M = 0;
      if (nsigma==0 && noise_type!=3) return *this;
      if (nsigma<0 || noise_type==2) m = (Tfloat)min_max(M);
      if (nsigma<0) nsigma = (Tfloat)(-nsigma*(M-m)/100.0);
      switch (noise_type) {
      case 0 : { // Gaussian noise
        cimg_rof(*this,ptrd,T) {
          Tfloat val = (Tfloat)(*ptrd + nsigma*cimg::grand());
          if (val>vmax) val = vmax;
          if (val<vmin) val = vmin;
          *ptrd = (T)val;
        }
      } break;
      case 1 : { // Uniform noise
        cimg_rof(*this,ptrd,T) {
          Tfloat val = (Tfloat)(*ptrd + nsigma*cimg::rand(-1,1));
          if (val>vmax) val = vmax;
          if (val<vmin) val = vmin;
          *ptrd = (T)val;
        }
      } break;
      case 2 : { // Salt & Pepper noise
        if (nsigma<0) nsigma = -nsigma;
        if (M==m) { m = 0; M = cimg::type<T>::is_float()?(Tfloat)1:(Tfloat)cimg::type<T>::max(); }
        cimg_rof(*this,ptrd,T) if (cimg::rand(100)<nsigma) *ptrd = (T)(cimg::rand()<0.5?M:m);
      } break;
      case 3 : { // Poisson Noise
        cimg_rof(*this,ptrd,T) *ptrd = (T)cimg::prand(*ptrd);
      } break;
      case 4 : { // Rice noise
        const Tfloat sqrt2 = (Tfloat)std::sqrt(2.0);
        cimg_rof(*this,ptrd,T) {
          const Tfloat
            val0 = (Tfloat)*ptrd/sqrt2,
            re = (Tfloat)(val0 + nsigma*cimg::grand()),
            im = (Tfloat)(val0 + nsigma*cimg::grand());
          Tfloat val = cimg::hypot(re,im);
          if (val>vmax) val = vmax;
          if (val<vmin) val = vmin;
          *ptrd = (T)val;
        }
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "noise(): Invalid specified noise type %d "
                                    "(should be { 0=gaussian | 1=uniform | 2=salt&Pepper | 3=poisson }).",
                                    cimg_instance,
                                    noise_type);
      }
      return *this;