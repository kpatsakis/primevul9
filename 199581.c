    **/
    static void FFT(CImg<T>& real, CImg<T>& imag, const bool is_invert=false, const unsigned int nb_threads=0) {
      if (!real)
        throw CImgInstanceException("CImgList<%s>::FFT(): Empty specified real part.",
                                    pixel_type());

      if (!imag) imag.assign(real._width,real._height,real._depth,real._spectrum,(T)0);
      if (!real.is_sameXYZC(imag))
        throw CImgInstanceException("CImgList<%s>::FFT(): Specified real part (%u,%u,%u,%u,%p) and "
                                    "imaginary part (%u,%u,%u,%u,%p) have different dimensions.",
                                    pixel_type(),
                                    real._width,real._height,real._depth,real._spectrum,real._data,
                                    imag._width,imag._height,imag._depth,imag._spectrum,imag._data);

#ifdef cimg_use_fftw3
      cimg::mutex(12);
#ifndef cimg_use_fftw3_singlethread
      const unsigned int _nb_threads = nb_threads?nb_threads:cimg::nb_cpus();
      static int fftw_st = fftw_init_threads();
      cimg::unused(fftw_st);
      fftw_plan_with_nthreads(_nb_threads);
#else
      cimg::unused(nb_threads);
#endif
      fftw_complex *data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*real._width*real._height*real._depth);
      if (!data_in) throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                                "for computing FFT of image (%u,%u,%u,%u).",
                                                pixel_type(),
                                                cimg::strbuffersize(sizeof(fftw_complex)*real._width*
                                                                    real._height*real._depth*real._spectrum),
                                                real._width,real._height,real._depth,real._spectrum);

      fftw_plan data_plan;
      const ulongT w = (ulongT)real._width, wh = w*real._height, whd = wh*real._depth;
      data_plan = fftw_plan_dft_3d(real._width,real._height,real._depth,data_in,data_in,
                                   is_invert?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
      cimg_forC(real,c) {
        T *ptrr = real.data(0,0,0,c), *ptri = imag.data(0,0,0,c);
        double *ptrd = (double*)data_in;
        for (unsigned int x = 0; x<real._width; ++x, ptrr-=wh - 1, ptri-=wh - 1)
          for (unsigned int y = 0; y<real._height; ++y, ptrr-=whd-w, ptri-=whd-w)
            for (unsigned int z = 0; z<real._depth; ++z, ptrr+=wh, ptri+=wh) {
              *(ptrd++) = (double)*ptrr; *(ptrd++) = (double)*ptri;
            }
        fftw_execute(data_plan);
        ptrd = (double*)data_in;
        ptrr = real.data(0,0,0,c);
        ptri = imag.data(0,0,0,c);
        if (!is_invert) for (unsigned int x = 0; x<real._width; ++x, ptrr-=wh - 1, ptri-=wh - 1)
          for (unsigned int y = 0; y<real._height; ++y, ptrr-=whd-w, ptri-=whd-w)
            for (unsigned int z = 0; z<real._depth; ++z, ptrr+=wh, ptri+=wh) {
              *ptrr = (T)*(ptrd++); *ptri = (T)*(ptrd++);
            }
        else for (unsigned int x = 0; x<real._width; ++x, ptrr-=wh - 1, ptri-=wh - 1)
          for (unsigned int y = 0; y<real._height; ++y, ptrr-=whd-w, ptri-=whd-w)
            for (unsigned int z = 0; z<real._depth; ++z, ptrr+=wh, ptri+=wh) {
              *ptrr = (T)(*(ptrd++)/whd); *ptri = (T)(*(ptrd++)/whd);
            }
      }
      fftw_destroy_plan(data_plan);
      fftw_free(data_in);
#ifndef cimg_use_fftw3_singlethread
      fftw_cleanup_threads();
#endif
      cimg::mutex(12,0);
#else
      cimg::unused(nb_threads);
      if (real._depth>1) FFT(real,imag,'z',is_invert);
      if (real._height>1) FFT(real,imag,'y',is_invert);
      if (real._width>1) FFT(real,imag,'x',is_invert);
#endif