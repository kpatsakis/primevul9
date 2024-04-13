    //! Crop image region \newinstance.
    CImg<T> get_crop(const int x0, const int y0, const int z0, const int c0,
                     const int x1, const int y1, const int z1, const int c1,
                     const unsigned int boundary_conditions=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "crop(): Empty instance.",
                                    cimg_instance);
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0,
        nz0 = z0<z1?z0:z1, nz1 = z0^z1^nz0,
        nc0 = c0<c1?c0:c1, nc1 = c0^c1^nc0;
      CImg<T> res(1U + nx1 - nx0,1U + ny1 - ny0,1U + nz1 - nz0,1U + nc1 - nc0);
      if (nx0<0 || nx1>=width() || ny0<0 || ny1>=height() || nz0<0 || nz1>=depth() || nc0<0 || nc1>=spectrum())
        switch (boundary_conditions) {
        case 3 : { // Mirror
          const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth(), s2 = 2*spectrum();
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=16 && _height*_depth*_spectrum>=4))
          cimg_forXYZC(res,x,y,z,c) {
            const int
              mx = cimg::mod(nx0 + x,w2),
              my = cimg::mod(ny0 + y,h2),
              mz = cimg::mod(nz0 + z,d2),
              mc = cimg::mod(nc0 + c,s2);
            res(x,y,z,c) = (*this)(mx<width()?mx:w2 - mx - 1,
                                   my<height()?my:h2 - my - 1,
                                   mz<depth()?mz:d2 - mz - 1,
                                   mc<spectrum()?mc:s2 - mc - 1);
          }
        } break;
        case 2 : { // Periodic
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=16 && _height*_depth*_spectrum>=4))
          cimg_forXYZC(res,x,y,z,c) {
            res(x,y,z,c) = (*this)(cimg::mod(nx0 + x,width()),cimg::mod(ny0 + y,height()),
                                   cimg::mod(nz0 + z,depth()),cimg::mod(nc0 + c,spectrum()));
          }
        } break;
        case 1 : // Neumann
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=16 && _height*_depth*_spectrum>=4))
          cimg_forXYZC(res,x,y,z,c) res(x,y,z,c) = _atXYZC(nx0 + x,ny0 + y,nz0 + z,nc0 + c);
          break;
        default : // Dirichlet
          res.fill((T)0).draw_image(-nx0,-ny0,-nz0,-nc0,*this);
        }
      else res.draw_image(-nx0,-ny0,-nz0,-nc0,*this);
      return res;