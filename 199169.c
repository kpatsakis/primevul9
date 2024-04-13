    template<typename tc, typename t>
    CImg<T>& draw_fill(const int x0, const int y0, const int z0,
                        const tc *const color, const float opacity,
                        CImg<t> &region,
                        const float tolerance = 0,
                        const bool is_high_connectivity = false) {
#define _draw_fill_push(x,y,z) if (N>=stack._width) stack.resize(2*N + 1,1,1,3,0); \
                               stack[N] = x; stack(N,1) = y; stack(N++,2) = z
#define _draw_fill_pop(x,y,z) x = stack[--N]; y = stack(N,1); z = stack(N,2)
#define _draw_fill_is_inside(x,y,z) !_region(x,y,z) && _draw_fill(x,y,z,ref,tolerance2)

      if (!containsXYZC(x0,y0,z0,0)) return *this;
      const float nopacity = cimg::abs((float)opacity), copacity = 1 - std::max((float)opacity,0.0f);
      const float tolerance2 = cimg::sqr(tolerance);
      const CImg<T> ref = get_vector_at(x0,y0,z0);
      CImg<uintT> stack(256,1,1,3);
      CImg<ucharT> _region(_width,_height,_depth,1,0);
      unsigned int N = 0;
      int x, y, z;

      _draw_fill_push(x0,y0,z0);
      while (N>0) {
        _draw_fill_pop(x,y,z);
        if (!_region(x,y,z)) {
          const int yp = y - 1, yn = y + 1, zp = z - 1, zn = z + 1;
          int xl = x, xr = x;

          // Using these booleans reduces the number of pushes drastically.
          bool is_yp = false, is_yn = false, is_zp = false, is_zn = false;
          for (int step = -1; step<2; step+=2) {
            while (x>=0 && x<width() && _draw_fill_is_inside(x,y,z)) {
              if (yp>=0 && _draw_fill_is_inside(x,yp,z)) {
                if (!is_yp) { _draw_fill_push(x,yp,z); is_yp = true; }
              } else is_yp = false;
              if (yn<height() && _draw_fill_is_inside(x,yn,z)) {
                if (!is_yn) { _draw_fill_push(x,yn,z); is_yn = true; }
              } else is_yn = false;
              if (depth()>1) {
                if (zp>=0 && _draw_fill_is_inside(x,y,zp)) {
                  if (!is_zp) { _draw_fill_push(x,y,zp); is_zp = true; }
                } else is_zp = false;
                if (zn<depth() && _draw_fill_is_inside(x,y,zn)) {
                  if (!is_zn) { _draw_fill_push(x,y,zn); is_zn = true; }
                } else is_zn = false;
              }
              if (is_high_connectivity) {
                const int xp = x - 1, xn = x + 1;
                if (yp>=0 && !is_yp) {
                  if (xp>=0 && _draw_fill_is_inside(xp,yp,z)) {
                    _draw_fill_push(xp,yp,z); if (step<0) is_yp = true;
                  }
                  if (xn<width() && _draw_fill_is_inside(xn,yp,z)) {
                    _draw_fill_push(xn,yp,z); if (step>0) is_yp = true;
                  }
                }
                if (yn<height() && !is_yn) {
                  if (xp>=0 && _draw_fill_is_inside(xp,yn,z)) {
                    _draw_fill_push(xp,yn,z); if (step<0) is_yn = true;
                  }
                  if (xn<width() && _draw_fill_is_inside(xn,yn,z)) {
                    _draw_fill_push(xn,yn,z); if (step>0) is_yn = true;
                  }
                }
                if (depth()>1) {
                  if (zp>=0 && !is_zp) {
                    if (xp>=0 && _draw_fill_is_inside(xp,y,zp)) {
                      _draw_fill_push(xp,y,zp); if (step<0) is_zp = true;
                    }
                    if (xn<width() && _draw_fill_is_inside(xn,y,zp)) {
                      _draw_fill_push(xn,y,zp); if (step>0) is_zp = true;
                    }

                    if (yp>=0 && !is_yp) {
                      if (_draw_fill_is_inside(x,yp,zp)) { _draw_fill_push(x,yp,zp); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yp,zp)) { _draw_fill_push(xp,yp,zp); }
                      if (xn<width() && _draw_fill_is_inside(xn,yp,zp)) { _draw_fill_push(xn,yp,zp); }
                    }
                    if (yn<height() && !is_yn) {
                      if (_draw_fill_is_inside(x,yn,zp)) { _draw_fill_push(x,yn,zp); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yn,zp)) { _draw_fill_push(xp,yn,zp); }
                      if (xn<width() && _draw_fill_is_inside(xn,yn,zp)) { _draw_fill_push(xn,yn,zp); }
                    }
                  }

                  if (zn<depth() && !is_zn) {
                    if (xp>=0 && _draw_fill_is_inside(xp,y,zn)) {
                      _draw_fill_push(xp,y,zn); if (step<0) is_zn = true;
                    }
                    if (xn<width() && _draw_fill_is_inside(xn,y,zn)) {
                      _draw_fill_push(xn,y,zn); if (step>0) is_zn = true;
                    }

                    if (yp>=0 && !is_yp) {
                      if (_draw_fill_is_inside(x,yp,zn)) { _draw_fill_push(x,yp,zn); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yp,zn)) { _draw_fill_push(xp,yp,zn); }
                      if (xn<width() && _draw_fill_is_inside(xn,yp,zn)) { _draw_fill_push(xn,yp,zn); }
                    }
                    if (yn<height() && !is_yn) {
                      if (_draw_fill_is_inside(x,yn,zn)) { _draw_fill_push(x,yn,zn); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yn,zn)) { _draw_fill_push(xp,yn,zn); }
                      if (xn<width() && _draw_fill_is_inside(xn,yn,zn)) { _draw_fill_push(xn,yn,zn); }
                    }
                  }
                }
              }
              x+=step;
            }
            if (step<0) { xl = ++x; x = xr + 1; is_yp = is_yn = is_zp = is_zn = false; }
            else xr = --x;
          }
          std::memset(_region.data(xl,y,z),1,xr - xl + 1);
          if (opacity==1) {
            if (sizeof(T)==1) {
              const int dx = xr - xl + 1;
              cimg_forC(*this,c) std::memset(data(xl,y,z,c),(int)color[c],dx);
            } else cimg_forC(*this,c) {
                const T val = (T)color[c];
                T *ptri = data(xl,y,z,c); for (int k = xl; k<=xr; ++k) *(ptri++) = val;
              }
          } else cimg_forC(*this,c) {
              const T val = (T)(color[c]*nopacity);
              T *ptri = data(xl,y,z,c); for (int k = xl; k<=xr; ++k) { *ptri = (T)(val + *ptri*copacity); ++ptri; }
            }
        }
      }
      _region.move_to(region);
      return *this;