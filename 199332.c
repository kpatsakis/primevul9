    //! Convert pixel values from YUV to RGB color spaces.
    CImg<T>& YUVtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "YUVtoRGB(): Instance is not a YUV image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=16384))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          Y = (Tfloat)p1[N],
          U = (Tfloat)p2[N],
          V = (Tfloat)p3[N],
          R = (Y + 1.140f*V)*255,
          G = (Y - 0.395f*U - 0.581f*V)*255,
          B = (Y + 2.032f*U)*255;
        p1[N] = (T)cimg::cut(R,0,255),
        p2[N] = (T)cimg::cut(G,0,255),
        p3[N] = (T)cimg::cut(B,0,255);
      }
      return *this;