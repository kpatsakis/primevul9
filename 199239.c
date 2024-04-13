    template<typename t>
    CImg<_cimg_Tt> get_dilate(const CImg<t>& kernel, const bool boundary_conditions=true,
                              const bool is_real=false) const {
      if (is_empty() || !kernel || (!is_real && kernel==0)) return *this;
      typedef _cimg_Tt Tt;
      CImg<Tt> res(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
      const int
        mx1 = kernel.width()/2, my1 = kernel.height()/2, mz1 = kernel.depth()/2,
        mx2 = kernel.width() - mx1 - 1, my2 = kernel.height() - my1 - 1, mz2 = kernel.depth() - mz1 - 1,
        mxe = width() - mx2, mye = height() - my2, mze = depth() - mz2;
      const bool
        is_inner_parallel = _width*_height*_depth>=32768,
        is_outer_parallel = res.size()>=32768;
      cimg::unused(is_inner_parallel,is_outer_parallel);
      _cimg_abort_init_omp;
      cimg_abort_init;
      cimg_pragma_openmp(parallel for cimg_openmp_if(!is_inner_parallel && is_outer_parallel))
      cimg_forC(res,c) _cimg_abort_try_omp {
        cimg_abort_test;
        const CImg<T> img = get_shared_channel(c%_spectrum);
        const CImg<t> K = kernel.get_shared_channel(c%kernel._spectrum);
        if (is_real) { // Real dilation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(is_inner_parallel))
          for (int z = mz1; z<mze; ++z)
            for (int y = my1; y<mye; ++y)
              for (int x = mx1; x<mxe; ++x) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm) {
                      const t mval = K(mx2 - xm,my2 - ym,mz2 - zm);
                      const Tt cval = (Tt)(img(x + xm,y + ym,z + zm) + mval);
                      if (cval>max_val) max_val = cval;
                    }
                res(x,y,z,c) = max_val;
              } _cimg_abort_catch_omp2
          if (boundary_conditions)
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
            cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
              cimg_abort_test2;
              for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm) {
                      const t mval = K(mx2 - xm,my2 - ym,mz2 - zm);
                      const Tt cval = (Tt)(img._atXYZ(x + xm,y + ym,z + zm) + mval);
                      if (cval>max_val) max_val = cval;
                    }
                res(x,y,z,c) = max_val;
              }
            } _cimg_abort_catch_omp2
          else
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
            cimg_forYZ(*this,y,z) _cimg_abort_try_omp2 {
              cimg_abort_test2;
              for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm) {
                      const t mval = K(mx2 - xm,my2 - ym,mz2 - zm);
                      const Tt cval = (Tt)(img.atXYZ(x + xm,y + ym,z + zm,0,(T)0) + mval);
                      if (cval>max_val) max_val = cval;
                    }
                res(x,y,z,c) = max_val;
              }
            } _cimg_abort_catch_omp2
        } else { // Binary dilation
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(is_inner_parallel))
          for (int z = mz1; z<mze; ++z)
            for (int y = my1; y<mye; ++y)
              for (int x = mx1; x<mxe; ++x) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm)
                      if (K(mx2 - xm,my2 - ym,mz2 - zm)) {
                        const Tt cval = (Tt)img(x + xm,y + ym,z + zm);
                        if (cval>max_val) max_val = cval;
                      }
                res(x,y,z,c) = max_val;
              } _cimg_abort_catch_omp2
          if (boundary_conditions)
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
            cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
              cimg_abort_test2;
              for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm)
                      if (K(mx2 - xm,my2 - ym,mz2 - zm)) {
                        const T cval = (Tt)img._atXYZ(x + xm,y + ym,z + zm);
                        if (cval>max_val) max_val = cval;
                      }
                res(x,y,z,c) = max_val;
              }
            } _cimg_abort_catch_omp2
          else
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
            cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
              cimg_abort_test2;
              for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm)
                      if (K(mx2 - xm,my2 - ym,mz2 - zm)) {
                        const T cval = (Tt)img.atXYZ(x + xm,y + ym,z + zm,0,(T)0);
                        if (cval>max_val) max_val = cval;
                      }
                res(x,y,z,c) = max_val;
              }
            } _cimg_abort_catch_omp2
        }
      } _cimg_abort_catch_omp
      cimg_abort_test;
      return res;