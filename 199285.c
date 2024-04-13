
    CImg<T>& _distance_core(longT (*const sep)(const longT, const longT, const longT *const),
                            longT (*const f)(const longT, const longT, const longT *const)) {
 // Check for g++ 4.9.X, as OpenMP seems to crash for this particular function. I have no clues why.
#define cimg_is_gcc49x (__GNUC__==4 && __GNUC_MINOR__==9)

      const ulongT wh = (ulongT)_width*_height;
#if defined(cimg_use_openmp) && !cimg_is_gcc49x
      cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
#endif
      cimg_forC(*this,c) {
        CImg<longT> g(_width), dt(_width), s(_width), t(_width);
        CImg<T> img = get_shared_channel(c);
#if defined(cimg_use_openmp) && !cimg_is_gcc49x
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=512 && _height*_depth>=16)
                           firstprivate(g,dt,s,t))
#endif
        cimg_forYZ(*this,y,z) { // Over X-direction.
          cimg_forX(*this,x) g[x] = (longT)img(x,y,z,0,wh);
          _distance_scan(_width,g,sep,f,s,t,dt);
          cimg_forX(*this,x) img(x,y,z,0,wh) = (T)dt[x];
        }
        if (_height>1) {
          g.assign(_height); dt.assign(_height); s.assign(_height); t.assign(_height);
#if defined(cimg_use_openmp) && !cimg_is_gcc49x
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_height>=512 && _width*_depth>=16)
                             firstprivate(g,dt,s,t))
#endif
          cimg_forXZ(*this,x,z) { // Over Y-direction.
            cimg_forY(*this,y) g[y] = (longT)img(x,y,z,0,wh);
            _distance_scan(_height,g,sep,f,s,t,dt);
            cimg_forY(*this,y) img(x,y,z,0,wh) = (T)dt[y];
          }
        }
        if (_depth>1) {
          g.assign(_depth); dt.assign(_depth); s.assign(_depth); t.assign(_depth);
#if defined(cimg_use_openmp) && !cimg_is_gcc49x
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_depth>=512 && _width*_height>=16)
                             firstprivate(g,dt,s,t))
#endif
          cimg_forXY(*this,x,y) { // Over Z-direction.
            cimg_forZ(*this,z) g[z] = (longT)img(x,y,z,0,wh);
            _distance_scan(_depth,g,sep,f,s,t,dt);
            cimg_forZ(*this,z) img(x,y,z,0,wh) = (T)dt[z];
          }
        }
      }
      return *this;