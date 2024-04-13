    **/
    CImg<T>& deriche(const float sigma, const unsigned int order=0, const char axis='x',
                     const bool boundary_conditions=true) {
#define _cimg_deriche_apply \
  CImg<Tfloat> Y(N); \
  Tfloat *ptrY = Y._data, yb = 0, yp = 0; \
  T xp = (T)0; \
  if (boundary_conditions) { xp = *ptrX; yb = yp = (Tfloat)(coefp*xp); } \
  for (int m = 0; m<N; ++m) { \
    const T xc = *ptrX; ptrX+=off; \
    const Tfloat yc = *(ptrY++) = (Tfloat)(a0*xc + a1*xp - b1*yp - b2*yb); \
    xp = xc; yb = yp; yp = yc; \
  } \
  T xn = (T)0, xa = (T)0; \
  Tfloat yn = 0, ya = 0; \
  if (boundary_conditions) { xn = xa = *(ptrX-off); yn = ya = (Tfloat)coefn*xn; } \
  for (int n = N - 1; n>=0; --n) { \
    const T xc = *(ptrX-=off); \
    const Tfloat yc = (Tfloat)(a2*xn + a3*xa - b1*yn - b2*ya); \
    xa = xn; xn = xc; ya = yn; yn = yc; \
    *ptrX = (T)(*(--ptrY)+yc); \
  }
      const char naxis = cimg::lowercase(axis);
      const float nsigma = sigma>=0?sigma:-sigma*(naxis=='x'?_width:naxis=='y'?_height:naxis=='z'?_depth:_spectrum)/100;
      if (is_empty() || (nsigma<0.1f && !order)) return *this;
      const float
        nnsigma = nsigma<0.1f?0.1f:nsigma,
        alpha = 1.695f/nnsigma,
        ema = (float)std::exp(-alpha),
        ema2 = (float)std::exp(-2*alpha),
        b1 = -2*ema,
        b2 = ema2;
      float a0 = 0, a1 = 0, a2 = 0, a3 = 0, coefp = 0, coefn = 0;
      switch (order) {
      case 0 : {
        const float k = (1-ema)*(1-ema)/(1 + 2*alpha*ema-ema2);
        a0 = k;
        a1 = k*(alpha - 1)*ema;
        a2 = k*(alpha + 1)*ema;
        a3 = -k*ema2;
      } break;
      case 1 : {
        const float k = -(1-ema)*(1-ema)*(1-ema)/(2*(ema + 1)*ema);
	a0 = a3 = 0;
	a1 = k*ema;
        a2 = -a1;
      } break;
      case 2 : {
        const float
          ea = (float)std::exp(-alpha),
          k = -(ema2 - 1)/(2*alpha*ema),
          kn = (-2*(-1 + 3*ea - 3*ea*ea + ea*ea*ea)/(3*ea + 1 + 3*ea*ea + ea*ea*ea));
        a0 = kn;
        a1 = -kn*(1 + k*alpha)*ema;
        a2 = kn*(1 - k*alpha)*ema;
        a3 = -kn*ema2;
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "deriche(): Invalid specified filter order %u "
                                    "(should be { 0=smoothing | 1=1st-derivative | 2=2nd-derivative }).",
                                    cimg_instance,
                                    order);
      }
      coefp = (a0 + a1)/(1 + b1 + b2);
      coefn = (a2 + a3)/(1 + b1 + b2);
      switch (naxis) {
      case 'x' : {
        const int N = width();
        const ulongT off = 1U;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c) { T *ptrX = data(0,y,z,c); _cimg_deriche_apply; }
      } break;
      case 'y' : {
        const int N = height();
        const ulongT off = (ulongT)_width;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c) { T *ptrX = data(x,0,z,c); _cimg_deriche_apply; }
      } break;
      case 'z' : {
        const int N = depth();
        const ulongT off = (ulongT)_width*_height;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c) { T *ptrX = data(x,y,0,c); _cimg_deriche_apply; }
      } break;
      default : {
        const int N = spectrum();
        const ulongT off = (ulongT)_width*_height*_depth;
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
        cimg_forXYZ(*this,x,y,z) { T *ptrX = data(x,y,z,0); _cimg_deriche_apply; }
      }
      }
      return *this;