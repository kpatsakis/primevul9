
    CImg<ulongT> _label(const unsigned int nb, const int *const dx,
                        const int *const dy, const int *const dz,
                        const Tfloat tolerance) const {
      CImg<ulongT> res(_width,_height,_depth,_spectrum);
      cimg_forC(*this,c) {
        CImg<ulongT> _res = res.get_shared_channel(c);

        // Init label numbers.
        ulongT *ptr = _res.data();
        cimg_foroff(_res,p) *(ptr++) = p;

        // For each neighbour-direction, label.
        for (unsigned int n = 0; n<nb; ++n) {
          const int _dx = dx[n], _dy = dy[n], _dz = dz[n];
          if (_dx || _dy || _dz) {
            const int
              x0 = _dx<0?-_dx:0,
              x1 = _dx<0?width():width() - _dx,
              y0 = _dy<0?-_dy:0,
              y1 = _dy<0?height():height() - _dy,
              z0 = _dz<0?-_dz:0,
              z1 = _dz<0?depth():depth() - _dz;
            const longT
              wh = (longT)width()*height(),
              whd = (longT)width()*height()*depth(),
              offset = _dz*wh + _dy*width() + _dx;
            for (longT z = z0, nz = z0 + _dz, pz = z0*wh; z<z1; ++z, ++nz, pz+=wh) {
              for (longT y = y0, ny = y0 + _dy, py = y0*width() + pz; y<y1; ++y, ++ny, py+=width()) {
                for (longT x = x0, nx = x0 + _dx, p = x0 + py; x<x1; ++x, ++nx, ++p) {
                  if (cimg::abs((Tfloat)(*this)(x,y,z,c,wh,whd) - (Tfloat)(*this)(nx,ny,nz,c,wh,whd))<=tolerance) {
                    const longT q = p + offset;
                    ulongT x, y;
                    for (x = (ulongT)(p<q?q:p), y = (ulongT)(p<q?p:q); x!=y && _res[x]!=x; ) {
                      x = _res[x]; if (x<y) cimg::swap(x,y);
                    }
                    if (x!=y) _res[x] = (ulongT)y;
                    for (ulongT _p = (ulongT)p; _p!=y; ) {
                      const ulongT h = _res[_p];
                      _res[_p] = (ulongT)y;
                      _p = h;
                    }
                    for (ulongT _q = (ulongT)q; _q!=y; ) {
                      const ulongT h = _res[_q];
                      _res[_q] = (ulongT)y;
                      _q = h;
                    }
                  }
                }
              }
            }
          }
        }

        // Resolve equivalences.
        ulongT counter = 0;
        ptr = _res.data();
        cimg_foroff(_res,p) { *ptr = *ptr==p?counter++:_res[*ptr]; ++ptr; }
      }
      return res;