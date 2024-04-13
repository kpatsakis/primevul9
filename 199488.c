    // Return a visualizable uchar8 image for display routines.
    CImg<ucharT> __get_select(const CImgDisplay& disp, const int normalization,
                              const int x, const int y, const int z) const {
      if (is_empty()) return CImg<ucharT>(1,1,1,1,0);
      const CImg<T> crop = get_shared_channels(0,std::min(2,spectrum() - 1));
      CImg<Tuchar> img2d;
      if (_depth>1) {
        const int mdisp = std::min(disp.screen_width(),disp.screen_height());
        if (depth()>mdisp) {
          crop.get_resize(-100,-100,mdisp,-100,0).move_to(img2d);
          img2d.projections2d(x,y,z*img2d._depth/_depth);
        } else crop.get_projections2d(x,y,z).move_to(img2d);
      } else CImg<Tuchar>(crop,false).move_to(img2d);

      // Check for inf and NaN values.
      if (cimg::type<T>::is_float() && normalization) {
        bool is_inf = false, is_nan = false;
        cimg_for(img2d,ptr,Tuchar)
          if (cimg::type<T>::is_inf(*ptr)) { is_inf = true; break; }
          else if (cimg::type<T>::is_nan(*ptr)) { is_nan = true; break; }
        if (is_inf || is_nan) {
          Tint m0 = (Tint)cimg::type<T>::max(), M0 = (Tint)cimg::type<T>::min();
          if (!normalization) { m0 = 0; M0 = 255; }
          else if (normalization==2) { m0 = (Tint)disp._min; M0 = (Tint)disp._max; }
          else
            cimg_for(img2d,ptr,Tuchar)
              if (!cimg::type<T>::is_inf(*ptr) && !cimg::type<T>::is_nan(*ptr)) {
                if (*ptr<(Tuchar)m0) m0 = *ptr;
                if (*ptr>(Tuchar)M0) M0 = *ptr;
              }
          const T
            val_minf = (T)(normalization==1 || normalization==3?m0 - (M0 - m0)*20 - 1:m0),
            val_pinf = (T)(normalization==1 || normalization==3?M0 + (M0 - m0)*20 + 1:M0);
          if (is_nan)
            cimg_for(img2d,ptr,Tuchar)
              if (cimg::type<T>::is_nan(*ptr)) *ptr = val_minf; // Replace NaN values.
          if (is_inf)
            cimg_for(img2d,ptr,Tuchar)
              if (cimg::type<T>::is_inf(*ptr)) *ptr = (float)*ptr<0?val_minf:val_pinf; // Replace +-inf values.
        }
      }

      switch (normalization) {
      case 1 : img2d.normalize((ucharT)0,(ucharT)255); break;
      case 2 : {
        const float m = disp._min, M = disp._max;
        (img2d-=m)*=255.0f/(M - m>0?M - m:1);
      } break;
      case 3 :
        if (cimg::type<T>::is_float()) img2d.normalize((ucharT)0,(ucharT)255);
        else {
          const float m = (float)cimg::type<T>::min(), M = (float)cimg::type<T>::max();
          (img2d-=m)*=255.0f/(M - m>0?M - m:1);
        } break;
      }
      if (img2d.spectrum()==2) img2d.channels(0,2);
      return img2d;