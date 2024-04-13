    template<typename tc>
    CImg<T>& _draw_scanline(const int x0, const int x1, const int y,
                            const tc *const color, const float opacity,
                            const float brightness,
                            const float nopacity, const float copacity, const ulongT whd) {
      static const T maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max());
      const int nx0 = x0>0?x0:0, nx1 = x1<width()?x1:width() - 1, dx = nx1 - nx0;
      if (dx>=0) {
        const tc *col = color;
        const ulongT off = whd - dx - 1;
        T *ptrd = data(nx0,y);
        if (opacity>=1) { // ** Opaque drawing **
          if (brightness==1) { // Brightness==1
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)*(col++);
                for (int x = dx; x>=0; --x) *(ptrd++) = val;
                ptrd+=off;
              } else cimg_forC(*this,c) {
                const T val = (T)*(col++);
                std::memset(ptrd,(int)val,dx + 1);
                ptrd+=whd;
              }
          } else if (brightness<1) { // Brightness<1
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)(*(col++)*brightness);
                for (int x = dx; x>=0; --x) *(ptrd++) = val;
                ptrd+=off;
              } else cimg_forC(*this,c) {
                const T val = (T)(*(col++)*brightness);
                std::memset(ptrd,(int)val,dx + 1);
                ptrd+=whd;
              }
          } else { // Brightness>1
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)((2-brightness)**(col++) + (brightness - 1)*maxval);
                for (int x = dx; x>=0; --x) *(ptrd++) = val;
                ptrd+=off;
              } else cimg_forC(*this,c) {
                const T val = (T)((2-brightness)**(col++) + (brightness - 1)*maxval);
                std::memset(ptrd,(int)val,dx + 1);
                ptrd+=whd;
              }
          }
        } else { // ** Transparent drawing **
          if (brightness==1) { // Brightness==1
            cimg_forC(*this,c) {
              const Tfloat val = *(col++)*nopacity;
              for (int x = dx; x>=0; --x) { *ptrd = (T)(val + *ptrd*copacity); ++ptrd; }
              ptrd+=off;
            }
          } else if (brightness<=1) { // Brightness<1
            cimg_forC(*this,c) {
              const Tfloat val = *(col++)*brightness*nopacity;
              for (int x = dx; x>=0; --x) { *ptrd = (T)(val + *ptrd*copacity); ++ptrd; }
              ptrd+=off;
            }
          } else { // Brightness>1
            cimg_forC(*this,c) {
              const Tfloat val = ((2-brightness)**(col++) + (brightness - 1)*maxval)*nopacity;
              for (int x = dx; x>=0; --x) { *ptrd = (T)(val + *ptrd*copacity); ++ptrd; }
              ptrd+=off;
            }
          }
        }
      }
      return *this;