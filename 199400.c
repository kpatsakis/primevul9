    CImg<typename CImg<t>::Tuint>
    get_index(const CImg<t>& colormap, const float dithering=1, const bool map_indexes=true) const {
      if (colormap._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "index(): Instance and specified colormap (%u,%u,%u,%u,%p) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    colormap._width,colormap._height,colormap._depth,colormap._spectrum,colormap._data);

      typedef typename CImg<t>::Tuint tuint;
      if (is_empty()) return CImg<tuint>();
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        pwhd = (ulongT)colormap._width*colormap._height*colormap._depth;
      CImg<tuint> res(_width,_height,_depth,map_indexes?_spectrum:1);
      tuint *ptrd = res._data;
      if (dithering>0) { // Dithered versions.
        const float ndithering = cimg::cut(dithering,0,1)/16;
        Tfloat valm = 0, valM = (Tfloat)max_min(valm);
        if (valm==valM && valm>=0 && valM<=255) { valm = 0; valM = 255; }
        CImg<Tfloat> cache = get_crop(-1,0,0,0,_width,1,0,_spectrum - 1);
        Tfloat *cache_current = cache.data(1,0,0,0), *cache_next = cache.data(1,1,0,0);
        const ulongT cwhd = (ulongT)cache._width*cache._height*cache._depth;
        switch (_spectrum) {
        case 1 : { // Optimized for scalars.
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc0 = cache_next; const T *ptrs0 = data(0,y + 1,z,0);
              cimg_forX(*this,x) *(ptrc0++) = (Tfloat)*(ptrs0++);
            }
            Tfloat *ptrs0 = cache_current, *ptrsn0 = cache_next;
            cimg_forX(*this,x) {
              const Tfloat _val0 = (Tfloat)*ptrs0, val0 = _val0<valm?valm:_val0>valM?valM:_val0;
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp_end = ptrp0 + pwhd; ptrp0<ptrp_end; ) {
                const Tfloat pval0 = (Tfloat)*(ptrp0++) - val0, dist = pval0*pval0;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              const Tfloat err0 = ((*(ptrs0++)=val0) - (Tfloat)*ptrmin0)*ndithering;
              *ptrs0+=7*err0; *(ptrsn0 - 1)+=3*err0; *(ptrsn0++)+=5*err0; *ptrsn0+=err0;
              if (map_indexes) *(ptrd++) = (tuint)*ptrmin0; else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        } break;
        case 2 : { // Optimized for 2d vectors.
          tuint *ptrd1 = ptrd + whd;
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc0 = cache_next, *ptrc1 = ptrc0 + cwhd;
              const T *ptrs0 = data(0,y + 1,z,0), *ptrs1 = ptrs0 + whd;
              cimg_forX(*this,x) { *(ptrc0++) = (Tfloat)*(ptrs0++); *(ptrc1++) = (Tfloat)*(ptrs1++); }
            }
            Tfloat
              *ptrs0 = cache_current, *ptrs1 = ptrs0 + cwhd,
              *ptrsn0 = cache_next, *ptrsn1 = ptrsn0 + cwhd;
            cimg_forX(*this,x) {
              const Tfloat
                _val0 = (Tfloat)*ptrs0, val0 = _val0<valm?valm:_val0>valM?valM:_val0,
                _val1 = (Tfloat)*ptrs1, val1 = _val1<valm?valm:_val1>valM?valM:_val1;
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0, pval1 = (Tfloat)*(ptrp1++) - val1,
                  dist = pval0*pval0 + pval1*pval1;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              const t *const ptrmin1 = ptrmin0 + pwhd;
              const Tfloat
                err0 = ((*(ptrs0++)=val0) - (Tfloat)*ptrmin0)*ndithering,
                err1 = ((*(ptrs1++)=val1) - (Tfloat)*ptrmin1)*ndithering;
              *ptrs0+=7*err0; *ptrs1+=7*err1;
              *(ptrsn0 - 1)+=3*err0; *(ptrsn1 - 1)+=3*err1;
              *(ptrsn0++)+=5*err0; *(ptrsn1++)+=5*err1;
              *ptrsn0+=err0; *ptrsn1+=err1;
              if (map_indexes) { *(ptrd++) = (tuint)*ptrmin0; *(ptrd1++) = (tuint)*ptrmin1; }
              else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        } break;
        case 3 : { // Optimized for 3d vectors (colors).
          tuint *ptrd1 = ptrd + whd, *ptrd2 = ptrd1 + whd;
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc0 = cache_next, *ptrc1 = ptrc0 + cwhd, *ptrc2 = ptrc1 + cwhd;
              const T *ptrs0 = data(0,y + 1,z,0), *ptrs1 = ptrs0 + whd, *ptrs2 = ptrs1 + whd;
              cimg_forX(*this,x) {
                *(ptrc0++) = (Tfloat)*(ptrs0++); *(ptrc1++) = (Tfloat)*(ptrs1++); *(ptrc2++) = (Tfloat)*(ptrs2++);
              }
            }
            Tfloat
              *ptrs0 = cache_current, *ptrs1 = ptrs0 + cwhd, *ptrs2 = ptrs1 + cwhd,
              *ptrsn0 = cache_next, *ptrsn1 = ptrsn0 + cwhd, *ptrsn2 = ptrsn1 + cwhd;
            cimg_forX(*this,x) {
              const Tfloat
                _val0 = (Tfloat)*ptrs0, val0 = _val0<valm?valm:_val0>valM?valM:_val0,
                _val1 = (Tfloat)*ptrs1, val1 = _val1<valm?valm:_val1>valM?valM:_val1,
                _val2 = (Tfloat)*ptrs2, val2 = _val2<valm?valm:_val2>valM?valM:_val2;
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp2 = ptrp1 + pwhd,
                     *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0,
                  pval1 = (Tfloat)*(ptrp1++) - val1,
                  pval2 = (Tfloat)*(ptrp2++) - val2,
                  dist = pval0*pval0 + pval1*pval1 + pval2*pval2;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              const t *const ptrmin1 = ptrmin0 + pwhd, *const ptrmin2 = ptrmin1 + pwhd;
              const Tfloat
                err0 = ((*(ptrs0++)=val0) - (Tfloat)*ptrmin0)*ndithering,
                err1 = ((*(ptrs1++)=val1) - (Tfloat)*ptrmin1)*ndithering,
                err2 = ((*(ptrs2++)=val2) - (Tfloat)*ptrmin2)*ndithering;

              *ptrs0+=7*err0; *ptrs1+=7*err1; *ptrs2+=7*err2;
              *(ptrsn0 - 1)+=3*err0; *(ptrsn1 - 1)+=3*err1; *(ptrsn2 - 1)+=3*err2;
              *(ptrsn0++)+=5*err0; *(ptrsn1++)+=5*err1; *(ptrsn2++)+=5*err2;
              *ptrsn0+=err0; *ptrsn1+=err1; *ptrsn2+=err2;

              if (map_indexes) {
                *(ptrd++) = (tuint)*ptrmin0; *(ptrd1++) = (tuint)*ptrmin1; *(ptrd2++) = (tuint)*ptrmin2;
              } else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        } break;
        default : // Generic version
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc = cache_next;
              cimg_forC(*this,c) {
                Tfloat *_ptrc = ptrc; const T *_ptrs = data(0,y + 1,z,c);
                cimg_forX(*this,x) *(_ptrc++) = (Tfloat)*(_ptrs++);
                ptrc+=cwhd;
              }
            }
            Tfloat *ptrs = cache_current, *ptrsn = cache_next;
            cimg_forX(*this,x) {
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin = colormap._data;
              for (const t *ptrp = colormap._data, *ptrp_end = ptrp + pwhd; ptrp<ptrp_end; ++ptrp) {
                Tfloat dist = 0; Tfloat *_ptrs = ptrs; const t *_ptrp = ptrp;
                cimg_forC(*this,c) {
                  const Tfloat _val = *_ptrs, val = _val<valm?valm:_val>valM?valM:_val;
                  dist+=cimg::sqr((*_ptrs=val) - (Tfloat)*_ptrp); _ptrs+=cwhd; _ptrp+=pwhd;
                }
                if (dist<distmin) { ptrmin = ptrp; distmin = dist; }
              }
              const t *_ptrmin = ptrmin; Tfloat *_ptrs = ptrs++, *_ptrsn = (ptrsn++) - 1;
              cimg_forC(*this,c) {
                const Tfloat err = (*(_ptrs++) - (Tfloat)*_ptrmin)*ndithering;
                *_ptrs+=7*err; *(_ptrsn++)+=3*err; *(_ptrsn++)+=5*err; *_ptrsn+=err;
                _ptrmin+=pwhd; _ptrs+=cwhd - 1; _ptrsn+=cwhd - 2;
              }
              if (map_indexes) {
                tuint *_ptrd = ptrd++;
                cimg_forC(*this,c) { *_ptrd = (tuint)*ptrmin; _ptrd+=whd; ptrmin+=pwhd; }
              }
              else *(ptrd++) = (tuint)(ptrmin - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        }
      } else { // Non-dithered versions
        switch (_spectrum) {
        case 1 : { // Optimized for scalars.
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=64 && _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z);
            for (const T *ptrs0 = data(0,y,z), *ptrs_end = ptrs0 + _width; ptrs0<ptrs_end; ) {
              const Tfloat val0 = (Tfloat)*(ptrs0++);
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp_end = ptrp0 + pwhd; ptrp0<ptrp_end; ) {
                const Tfloat pval0 = (Tfloat)*(ptrp0++) - val0, dist = pval0*pval0;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              if (map_indexes) *(ptrd++) = (tuint)*ptrmin0; else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
          }
        } break;
        case 2 : { // Optimized for 2d vectors.
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=64 && _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z), *ptrd1 = ptrd + whd;
            for (const T *ptrs0 = data(0,y,z), *ptrs1 = ptrs0 + whd, *ptrs_end = ptrs0 + _width; ptrs0<ptrs_end; ) {
              const Tfloat val0 = (Tfloat)*(ptrs0++), val1 = (Tfloat)*(ptrs1++);
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0, pval1 = (Tfloat)*(ptrp1++) - val1,
                  dist = pval0*pval0 + pval1*pval1;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              if (map_indexes) { *(ptrd++) = (tuint)*ptrmin0; *(ptrd1++) = (tuint)*(ptrmin0 + pwhd); }
              else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
          }
        } break;
        case 3 : { // Optimized for 3d vectors (colors).
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=64 && _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z), *ptrd1 = ptrd + whd, *ptrd2 = ptrd1 + whd;
            for (const T *ptrs0 = data(0,y,z), *ptrs1 = ptrs0 + whd, *ptrs2 = ptrs1 + whd,
                   *ptrs_end = ptrs0 + _width; ptrs0<ptrs_end; ) {
              const Tfloat val0 = (Tfloat)*(ptrs0++), val1 = (Tfloat)*(ptrs1++), val2 = (Tfloat)*(ptrs2++);
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp2 = ptrp1 + pwhd,
                     *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0,
                  pval1 = (Tfloat)*(ptrp1++) - val1,
                  pval2 = (Tfloat)*(ptrp2++) - val2,
                  dist = pval0*pval0 + pval1*pval1 + pval2*pval2;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              if (map_indexes) {
                *(ptrd++) = (tuint)*ptrmin0;
                *(ptrd1++) = (tuint)*(ptrmin0 + pwhd);
                *(ptrd2++) = (tuint)*(ptrmin0 + 2*pwhd);
              } else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
          }
        } break;
        default : // Generic version.
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=64 && _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z);
            for (const T *ptrs = data(0,y,z), *ptrs_end = ptrs + _width; ptrs<ptrs_end; ++ptrs) {
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin = colormap._data;
              for (const t *ptrp = colormap._data, *ptrp_end = ptrp + pwhd; ptrp<ptrp_end; ++ptrp) {
                Tfloat dist = 0; const T *_ptrs = ptrs; const t *_ptrp = ptrp;
                cimg_forC(*this,c) { dist+=cimg::sqr((Tfloat)*_ptrs - (Tfloat)*_ptrp); _ptrs+=whd; _ptrp+=pwhd; }
                if (dist<distmin) { ptrmin = ptrp; distmin = dist; }
              }
              if (map_indexes) {
                tuint *_ptrd = ptrd++;
                cimg_forC(*this,c) { *_ptrd = (tuint)*ptrmin; _ptrd+=whd; ptrmin+=pwhd; }
              }
              else *(ptrd++) = (tuint)(ptrmin - colormap._data);
            }
          }
        }
      }
      return res;