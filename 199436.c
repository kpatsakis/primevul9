    **/
    CImg<T>& draw_plasma(const float alpha=1, const float beta=0, const unsigned int scale=8) {
      if (is_empty()) return *this;
      const int w = width(), h = height();
      const Tfloat m = (Tfloat)cimg::type<T>::min(), M = (Tfloat)cimg::type<T>::max();
      cimg_forZC(*this,z,c) {
        CImg<T> ref = get_shared_slice(z,c);
        for (int delta = 1<<std::min(scale,31U); delta>1; delta>>=1) {
          const int delta2 = delta>>1;
          const float r = alpha*delta + beta;

          // Square step.
          for (int y0 = 0; y0<h; y0+=delta)
            for (int x0 = 0; x0<w; x0+=delta) {
              const int x1 = (x0 + delta)%w, y1 = (y0 + delta)%h, xc = (x0 + delta2)%w, yc = (y0 + delta2)%h;
              const Tfloat val = (Tfloat)(0.25f*(ref(x0,y0) + ref(x0,y1) + ref(x0,y1) + ref(x1,y1)) +
                                          r*cimg::rand(-1,1));
              ref(xc,yc) = (T)(val<m?m:val>M?M:val);
            }

          // Diamond steps.
          for (int y = -delta2; y<h; y+=delta)
            for (int x0=0; x0<w; x0+=delta) {
              const int y0 = cimg::mod(y,h), x1 = (x0 + delta)%w, y1 = (y + delta)%h,
                xc = (x0 + delta2)%w, yc = (y + delta2)%h;
              const Tfloat val = (Tfloat)(0.25f*(ref(xc,y0) + ref(x0,yc) + ref(xc,y1) + ref(x1,yc)) +
                                          r*cimg::rand(-1,1));
              ref(xc,yc) = (T)(val<m?m:val>M?M:val);
            }
          for (int y0 = 0; y0<h; y0+=delta)
            for (int x = -delta2; x<w; x+=delta) {
              const int x0 = cimg::mod(x,w), x1 = (x + delta)%w, y1 = (y0 + delta)%h,
                xc = (x + delta2)%w, yc = (y0 + delta2)%h;
              const Tfloat val = (Tfloat)(0.25f*(ref(xc,y0) + ref(x0,yc) + ref(xc,y1) + ref(x1,yc)) +
                                          r*cimg::rand(-1,1));
              ref(xc,yc) = (T)(val<m?m:val>M?M:val);
            }
          for (int y = -delta2; y<h; y+=delta)
            for (int x = -delta2; x<w; x+=delta) {
              const int x0 = cimg::mod(x,w), y0 = cimg::mod(y,h), x1 = (x + delta)%w, y1 = (y + delta)%h,
                xc = (x + delta2)%w, yc = (y + delta2)%h;
              const Tfloat val = (Tfloat)(0.25f*(ref(xc,y0) + ref(x0,yc) + ref(xc,y1) + ref(x1,yc)) +
                                          r*cimg::rand(-1,1));
                ref(xc,yc) = (T)(val<m?m:val>M?M:val);
            }
        }
      }
      return *this;