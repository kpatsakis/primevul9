    **/
    static void FFT(CImg<T>& real, CImg<T>& imag, const char axis, const bool is_invert=false) {
      if (!real)
        throw CImgInstanceException("CImg<%s>::FFT(): Specified real part is empty.",
                                    pixel_type());

      if (!imag) imag.assign(real._width,real._height,real._depth,real._spectrum,(T)0);
      if (!real.is_sameXYZC(imag))
        throw CImgInstanceException("CImg<%s>::FFT(): Specified real part (%u,%u,%u,%u,%p) and "
                                    "imaginary part (%u,%u,%u,%u,%p) have different dimensions.",
                                    pixel_type(),
                                    real._width,real._height,real._depth,real._spectrum,real._data,
                                    imag._width,imag._height,imag._depth,imag._spectrum,imag._data);
#ifdef cimg_use_fftw3
      cimg::mutex(12);
      fftw_complex *data_in;
      fftw_plan data_plan;

      switch (cimg::lowercase(axis)) {
      case 'x' : { // Fourier along X, using FFTW library.
        data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*real._width);
        if (!data_in) throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                                  "for computing FFT of image (%u,%u,%u,%u) along the X-axis.",
                                                  pixel_type(),
                                                  cimg::strbuffersize(sizeof(fftw_complex)*real._width),
                                                  real._width,real._height,real._depth,real._spectrum);

        data_plan = fftw_plan_dft_1d(real._width,data_in,data_in,is_invert?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
        cimg_forYZC(real,y,z,c) {
          T *ptrr = real.data(0,y,z,c), *ptri = imag.data(0,y,z,c);
          double *ptrd = (double*)data_in;
          cimg_forX(real,x) { *(ptrd++) = (double)*(ptrr++); *(ptrd++) = (double)*(ptri++); }
          fftw_execute(data_plan);
          const unsigned int fact = real._width;
          if (is_invert) cimg_forX(real,x) { *(--ptri) = (T)(*(--ptrd)/fact); *(--ptrr) = (T)(*(--ptrd)/fact); }
          else cimg_forX(real,x) { *(--ptri) = (T)*(--ptrd); *(--ptrr) = (T)*(--ptrd); }
        }
      } break;
      case 'y' : { // Fourier along Y, using FFTW library.
        data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * real._height);
        if (!data_in) throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                                  "for computing FFT of image (%u,%u,%u,%u) along the Y-axis.",
                                                  pixel_type(),
                                                  cimg::strbuffersize(sizeof(fftw_complex)*real._height),
                                                  real._width,real._height,real._depth,real._spectrum);

        data_plan = fftw_plan_dft_1d(real._height,data_in,data_in,is_invert?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
        const unsigned int off = real._width;
        cimg_forXZC(real,x,z,c) {
          T *ptrr = real.data(x,0,z,c), *ptri = imag.data(x,0,z,c);
          double *ptrd = (double*)data_in;
          cimg_forY(real,y) { *(ptrd++) = (double)*ptrr; *(ptrd++) = (double)*ptri; ptrr+=off; ptri+=off; }
          fftw_execute(data_plan);
          const unsigned int fact = real._height;
          if (is_invert)
            cimg_forY(real,y) { ptrr-=off; ptri-=off; *ptri = (T)(*(--ptrd)/fact); *ptrr = (T)(*(--ptrd)/fact); }
          else cimg_forY(real,y) { ptrr-=off; ptri-=off; *ptri = (T)*(--ptrd); *ptrr = (T)*(--ptrd); }
        }
      } break;
      case 'z' : { // Fourier along Z, using FFTW library.
        data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * real._depth);
        if (!data_in) throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                                  "for computing FFT of image (%u,%u,%u,%u) along the Z-axis.",
                                                  pixel_type(),
                                                  cimg::strbuffersize(sizeof(fftw_complex)*real._depth),
                                                  real._width,real._height,real._depth,real._spectrum);

        data_plan = fftw_plan_dft_1d(real._depth,data_in,data_in,is_invert?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
        const ulongT off = (ulongT)real._width*real._height;
        cimg_forXYC(real,x,y,c) {
          T *ptrr = real.data(x,y,0,c), *ptri = imag.data(x,y,0,c);
          double *ptrd = (double*)data_in;
          cimg_forZ(real,z) { *(ptrd++) = (double)*ptrr; *(ptrd++) = (double)*ptri; ptrr+=off; ptri+=off; }
          fftw_execute(data_plan);
          const unsigned int fact = real._depth;
          if (is_invert)
            cimg_forZ(real,z) { ptrr-=off; ptri-=off; *ptri = (T)(*(--ptrd)/fact); *ptrr = (T)(*(--ptrd)/fact); }
          else cimg_forZ(real,z) { ptrr-=off; ptri-=off; *ptri = (T)*(--ptrd); *ptrr = (T)*(--ptrd); }
        }
      } break;
      default :
        throw CImgArgumentException("CImgList<%s>::FFT(): Invalid specified axis '%c' for real and imaginary parts "
                                    "(%u,%u,%u,%u) "
                                    "(should be { x | y | z }).",
                                    pixel_type(),axis,
                                    real._width,real._height,real._depth,real._spectrum);
      }
      fftw_destroy_plan(data_plan);
      fftw_free(data_in);
      cimg::mutex(12,0);
#else
      switch (cimg::lowercase(axis)) {
      case 'x' : { // Fourier along X, using built-in functions.
        const unsigned int N = real._width, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the X-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forYZC(real,y,z,c) {
              cimg::swap(real(i,y,z,c),real(j,y,z,c));
              cimg::swap(imag(i,y,z,c),imag(j,y,z,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(ri,y,z,c),real(rj,y,z,c));
                cimg::swap(imag(ri,y,z,c),imag(rj,y,z,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = delta>>1;
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_invert?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forYZC(real,y,z,c) {
                T &ir = real(j,y,z,c), &ii = imag(j,y,z,c), &nir = real(nj,y,z,c), &nii = imag(nj,y,z,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_invert) { real/=N; imag/=N; }
      } break;
      case 'y' : { // Fourier along Y, using built-in functions.
        const unsigned int N = real._height, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the Y-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forXZC(real,x,z,c) {
              cimg::swap(real(x,i,z,c),real(x,j,z,c));
              cimg::swap(imag(x,i,z,c),imag(x,j,z,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(x,ri,z,c),real(x,rj,z,c));
                cimg::swap(imag(x,ri,z,c),imag(x,rj,z,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = (delta>>1);
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_invert?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forXZC(real,x,z,c) {
                T &ir = real(x,j,z,c), &ii = imag(x,j,z,c), &nir = real(x,nj,z,c), &nii = imag(x,nj,z,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_invert) { real/=N; imag/=N; }
      } break;
      case 'z' : { // Fourier along Z, using built-in functions.
        const unsigned int N = real._depth, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the Z-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forXYC(real,x,y,c) {
              cimg::swap(real(x,y,i,c),real(x,y,j,c));
              cimg::swap(imag(x,y,i,c),imag(x,y,j,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(x,y,ri,c),real(x,y,rj,c));
                cimg::swap(imag(x,y,ri,c),imag(x,y,rj,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = (delta>>1);
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_invert?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forXYC(real,x,y,c) {
                T &ir = real(x,y,j,c), &ii = imag(x,y,j,c), &nir = real(x,y,nj,c), &nii = imag(x,y,nj,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_invert) { real/=N; imag/=N; }
      } break;
      default :
        throw CImgArgumentException("CImgList<%s>::FFT(): Invalid specified axis '%c' for real and imaginary parts "
                                    "(%u,%u,%u,%u) "
                                    "(should be { x | y | z }).",
                                    pixel_type(),axis,
                                    real._width,real._height,real._depth,real._spectrum);
      }
#endif