    */
    static void _cimg_blur_box_apply(T *ptr, const float boxsize, const int N, const ulongT off,
                                     const int order, const bool boundary_conditions,
                                     const unsigned int nb_iter) {
      // Smooth.
      if (boxsize>1 && nb_iter) {
        const int w2 = (int)(boxsize - 1)/2;
        const unsigned int winsize = 2*w2 + 1U;
        const double frac = (boxsize - winsize)/2.;
        CImg<T> win(winsize);
        for (unsigned int iter = 0; iter<nb_iter; ++iter) {
          Tdouble sum = 0; // window sum
          for (int x = -w2; x<=w2; ++x) {
            win[x + w2] = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,x);
            sum+=win[x + w2];
          }
          int ifirst = 0, ilast = 2*w2;
          T
            prev = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,-w2 - 1),
            next = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,w2 + 1);
          for (int x = 0; x < N - 1; ++x) {
            const double sum2 = sum + frac * (prev + next);
            ptr[x*off] = (T)(sum2/boxsize);
            prev = win[ifirst];
            sum-=prev;
            ifirst = (int)((ifirst + 1)%winsize);
            ilast = (int)((ilast + 1)%winsize);
            win[ilast] = next;
            sum+=next;
            next = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,x + w2 + 2);
          }
          const double sum2 = sum + frac * (prev + next);
          ptr[(N - 1)*off] = (T)(sum2/boxsize);
        }
      }

      // Derive.
      switch (order) {
      case 0 :
        break;
      case 1 : {
        Tfloat
          p = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,-1),
          c = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,0),
          n = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,1);
        for (int x = 0; x<N - 1; ++x) {
          ptr[x*off] = (T)((n-p)/2.0);
          p = c;
          c = n;
          n = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,x + 2);
        }
        ptr[(N - 1)*off] = (T)((n-p)/2.0);
      } break;
      case 2: {
        Tfloat
          p = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,-1),
          c = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,0),
          n = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,1);
        for (int x = 0; x<N - 1; ++x) {
          ptr[x*off] = (T)(n - 2*c + p);
          p = c;
          c = n;
          n = __cimg_blur_box_apply(ptr,N,off,boundary_conditions,x + 2);
        }
        ptr[(N - 1)*off] = (T)(n - 2*c + p);
      } break;
      }