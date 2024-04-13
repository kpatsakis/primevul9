    template<typename t>
    CImg<t> get_map(const CImg<t>& colormap, const unsigned int boundary_conditions=0) const {
      if (_spectrum!=1 && colormap._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "map(): Instance and specified colormap (%u,%u,%u,%u,%p) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    colormap._width,colormap._height,colormap._depth,colormap._spectrum,colormap._data);

      const ulongT
        whd = (ulongT)_width*_height*_depth,
        cwhd = (ulongT)colormap._width*colormap._height*colormap._depth,
        cwhd2 = 2*cwhd;
      CImg<t> res(_width,_height,_depth,colormap._spectrum==1?_spectrum:colormap._spectrum);
      switch (colormap._spectrum) {

      case 1 : { // Optimized for scalars
        const T *ptrs = _data;
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_for(res,ptrd,t) {
            const ulongT ind = ((ulongT)*(ptrs++))%cwhd2;
            *ptrd = colormap[ind<cwhd?ind:cwhd2 - ind - 1];
          }
          break;
        case 2 : // Periodic
          cimg_for(res,ptrd,t) {
            const ulongT ind = (ulongT)*(ptrs++);
            *ptrd = colormap[ind%cwhd];
          } break;
        case 1 : // Neumann
          cimg_for(res,ptrd,t) {
            const longT ind = (longT)*(ptrs++);
            *ptrd = colormap[cimg::cut(ind,(longT)0,(longT)cwhd - 1)];
          } break;
        default : // Dirichlet
          cimg_for(res,ptrd,t) {
            const ulongT ind = (ulongT)*(ptrs++);
            *ptrd = ind<cwhd?colormap[ind]:(t)0;
          }
        }
      } break;

      case 2 : { // Optimized for 2d vectors.
        const t *const ptrp0 = colormap._data, *ptrp1 = ptrp0 + cwhd;
        t *ptrd0 = res._data, *ptrd1 = ptrd0 + whd;
        switch (boundary_conditions) {
        case 3 : // Mirror
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT
              _ind = ((ulongT)*(ptrs++))%cwhd2,
              ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
            *(ptrd0++) = ptrp0[ind]; *(ptrd1++) = ptrp1[ind];
          }
          break;
        case 2 : // Periodic
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT ind = ((ulongT)*(ptrs++))%cwhd;
            *(ptrd0++) = ptrp0[ind]; *(ptrd1++) = ptrp1[ind];
          }
          break;
        case 1 : // Neumann
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const longT ind = cimg::cut((longT)*(ptrs++),(longT)0,(longT)cwhd - 1);
            *(ptrd0++) = ptrp0[ind]; *(ptrd1++) = ptrp1[ind];
          }
          break;
        default : // Dirichlet
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT ind = (ulongT)*(ptrs++);
            const bool is_in = ind<cwhd;
            *(ptrd0++) = is_in?ptrp0[ind]:(t)0; *(ptrd1++) = is_in?ptrp1[ind]:(t)0;
          }
        }
      } break;

      case 3 : { // Optimized for 3d vectors (colors).
        const t *const ptrp0 = colormap._data, *ptrp1 = ptrp0 + cwhd, *ptrp2 = ptrp1 + cwhd;
        t *ptrd0 = res._data, *ptrd1 = ptrd0 + whd, *ptrd2 = ptrd1 + whd;
        switch (boundary_conditions) {
        case 3 : // Mirror
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT
              _ind = ((ulongT)*(ptrs++))%cwhd2,
              ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
            *(ptrd0++) = ptrp0[ind]; *(ptrd1++) = ptrp1[ind]; *(ptrd2++) = ptrp2[ind];
          } break;
        case 2 : // Periodic
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT ind = ((ulongT)*(ptrs++))%cwhd;
            *(ptrd0++) = ptrp0[ind]; *(ptrd1++) = ptrp1[ind]; *(ptrd2++) = ptrp2[ind];
          } break;
        case 1 : // Neumann
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const longT ind = cimg::cut((longT)*(ptrs++),(longT)0,(longT)cwhd - 1);
            *(ptrd0++) = ptrp0[ind]; *(ptrd1++) = ptrp1[ind]; *(ptrd2++) = ptrp2[ind];
          } break;
        default : // Dirichlet
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT ind = (ulongT)*(ptrs++);
            const bool is_in = ind<cwhd;
            *(ptrd0++) = is_in?ptrp0[ind]:(t)0; *(ptrd1++) = is_in?ptrp1[ind]:(t)0; *(ptrd2++) = is_in?ptrp2[ind]:(t)0;
          }
        }
      } break;

      default : { // Generic version.
        t *ptrd = res._data;
        switch (boundary_conditions) {
        case 3 : // Mirror
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT
              _ind = ((ulongT)*(ptrs++))%cwhd,
              ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
            const t *ptrp = colormap._data + ind;
            t *_ptrd = ptrd++; cimg_forC(res,c) { *_ptrd = *ptrp; _ptrd+=whd; ptrp+=cwhd; }
          } break;
        case 2 : // Periodic
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT ind = ((ulongT)*(ptrs++))%cwhd;
            const t *ptrp = colormap._data + ind;
            t *_ptrd = ptrd++; cimg_forC(res,c) { *_ptrd = *ptrp; _ptrd+=whd; ptrp+=cwhd; }
          } break;
        case 1 : // Neumann
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const longT ind = cimg::cut((longT)*(ptrs++),(longT)0,(longT)cwhd - 1);
            const t *ptrp = colormap._data + ind;
            t *_ptrd = ptrd++; cimg_forC(res,c) { *_ptrd = *ptrp; _ptrd+=whd; ptrp+=cwhd; }
          } break;
        default : // Dirichlet
          for (const T *ptrs = _data, *ptrs_end = ptrs + whd; ptrs<ptrs_end; ) {
            const ulongT ind = (ulongT)*(ptrs++);
            const bool is_in = ind<cwhd;
            if (is_in) {
              const t *ptrp = colormap._data + ind;
              t *_ptrd = ptrd++; cimg_forC(res,c) { *_ptrd = *ptrp; _ptrd+=whd; ptrp+=cwhd; }
            } else {
              t *_ptrd = ptrd++; cimg_forC(res,c) { *_ptrd = (t)0; _ptrd+=whd; }
            }
          }
        }
      }
      }
      return res;