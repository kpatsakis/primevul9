    **/
    CImg<T>& erode(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) {
      if (is_empty() || (sx==1 && sy==1 && sz==1)) return *this;
      if (sx>1 && _width>1) { // Along X-axis.
        const int L = width(), off = 1, s = (int)sx, _s2 = s/2 + 1, _s1 = s - _s2, s1 = _s1>L?L:_s1, s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forYZC(*this,y,z,c) {
          T *const ptrdb = buf._data, *ptrd = buf._data, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(0,y,z,c), *ptrs = ptrsb, *const ptrse = ptrs + L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val<=cur) { cur = val; is_first = false; }}
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(0,y,z,c); cur = std::min(cur,*ptrse); cimg_forX(buf,x) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval<cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval<cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val<=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val<cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val<cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(0,y,z,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }

      if (sy>1 && _height>1) { // Along Y-axis.
        const int L = height(), off = width(), s = (int)sy, _s2 = s/2 + 1, _s1 = s - _s2, s1 = _s1>L?L:_s1,
          s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forXZC(*this,x,z,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(x,0,z,c), *ptrs = ptrsb, *const ptrse = ptrs + L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(x,0,z,c); cur = std::min(cur,*ptrse); cimg_forX(buf,x) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval<cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval<cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val<=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val<cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val<cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(x,0,z,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }

      if (sz>1 && _depth>1) { // Along Z-axis.
        const int L = depth(), off = width()*height(), s = (int)sz, _s2 = s/2 + 1, _s1 = s - _s2, s1 = _s1>L?L:_s1,
          s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forXYC(*this,x,y,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(x,y,0,c), *ptrs = ptrsb, *const ptrse = ptrs + L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(x,y,0,c); cur = std::min(cur,*ptrse); cimg_forX(buf,x) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval<cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval<cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val<=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val<cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val<cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(x,y,0,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }
      return *this;