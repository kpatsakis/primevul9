    template<typename t>
    CImg<_cimg_Ttfloat> _correlate(const CImg<t>& kernel, const bool boundary_conditions,
                                   const bool is_normalized, const bool is_convolution) const {
      if (is_empty() || !kernel) return *this;
      typedef _cimg_Ttfloat Ttfloat;
      CImg<Ttfloat> res;
      const ulongT
        res_whd = (ulongT)_width*_height*_depth,
        res_size = res_whd*std::max(_spectrum,kernel._spectrum);
      const bool
        is_inner_parallel = _width*_height*_depth>=32768,
        is_outer_parallel = res_size>=32768;
      _cimg_abort_init_omp;
      cimg_abort_init;

      if (kernel._width==kernel._height &&
          ((kernel._depth==1 && kernel._width<=6) || (kernel._depth==kernel._width && kernel._width<=3))) {

        // Special optimization done for 2x2, 3x3, 4x4, 5x5, 6x6, 2x2x2 and 3x3x3 kernel.
        if (!boundary_conditions && res_whd<=3000*3000) { // Dirichlet boundaries
          // For relatively small images, adding a zero border then use optimized NxN convolution loops is faster.
          res = (kernel._depth==1?get_crop(-1,-1,_width,_height):get_crop(-1,-1,-1,_width,_height,_depth)).
            _correlate(kernel,true,is_normalized,is_convolution);
          if (kernel._depth==1) res.crop(1,1,res._width - 2,res._height - 2);
          else res.crop(1,1,1,res._width - 2,res._height - 2,res._depth - 2);

        } else { // Neumann boundaries
          res.assign(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
          cimg::unused(is_inner_parallel,is_outer_parallel);
          CImg<t> _kernel;
          if (is_convolution) { // Add empty column/row/slice to shift kernel center in case of convolution
            const int dw = !(kernel.width()%2), dh = !(kernel.height()%2), dd = !(kernel.depth()%2);
            if (dw || dh || dd)
              kernel.get_resize(kernel.width() + dw,kernel.height() + dh,kernel.depth() + dd,-100,0,0).
                move_to(_kernel);
          }
          if (!_kernel) _kernel = kernel.get_shared();

          switch (_kernel._depth) {
          case 3 : {
            cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
              cimg_forC(res,c) {
              cimg_abort_test;
              const CImg<T> img = get_shared_channel(c%_spectrum);
              const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
              CImg<T> I(27);
              Ttfloat *ptrd = res.data(0,0,0,c);
              if (is_normalized) {
                const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                cimg_for3x3x3(img,x,y,z,0,I,T) {
                  const Ttfloat N = M*(I[ 0]*I[ 0] + I[ 1]*I[ 1] + I[ 2]*I[ 2] +
                                       I[ 3]*I[ 3] + I[ 4]*I[ 4] + I[ 5]*I[ 5] +
                                       I[ 6]*I[ 6] + I[ 7]*I[ 7] + I[ 8]*I[ 8] +
                                       I[ 9]*I[ 9] + I[10]*I[10] + I[11]*I[11] +
                                       I[12]*I[12] + I[13]*I[13] + I[14]*I[14] +
                                       I[15]*I[15] + I[16]*I[16] + I[17]*I[17] +
                                       I[18]*I[18] + I[19]*I[19] + I[20]*I[20] +
                                       I[21]*I[21] + I[22]*I[22] + I[23]*I[23] +
                                       I[24]*I[24] + I[25]*I[25] + I[26]*I[26]);
                  *(ptrd++) = (Ttfloat)(N?(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] +
                                           I[ 3]*K[ 3] + I[ 4]*K[ 4] + I[ 5]*K[ 5] +
                                           I[ 6]*K[ 6] + I[ 7]*K[ 7] + I[ 8]*K[ 8] +
                                           I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                           I[12]*K[12] + I[13]*K[13] + I[14]*K[14] +
                                           I[15]*K[15] + I[16]*K[16] + I[17]*K[17] +
                                           I[18]*K[18] + I[19]*K[19] + I[20]*K[20] +
                                           I[21]*K[21] + I[22]*K[22] + I[23]*K[23] +
                                           I[24]*K[24] + I[25]*K[25] + I[26]*K[26])/std::sqrt(N):0);
                }
              } else cimg_for3x3x3(img,x,y,z,0,I,T)
                       *(ptrd++) = (Ttfloat)(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] +
                                             I[ 3]*K[ 3] + I[ 4]*K[ 4] + I[ 5]*K[ 5] +
                                             I[ 6]*K[ 6] + I[ 7]*K[ 7] + I[ 8]*K[ 8] +
                                             I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                             I[12]*K[12] + I[13]*K[13] + I[14]*K[14] +
                                             I[15]*K[15] + I[16]*K[16] + I[17]*K[17] +
                                             I[18]*K[18] + I[19]*K[19] + I[20]*K[20] +
                                             I[21]*K[21] + I[22]*K[22] + I[23]*K[23] +
                                             I[24]*K[24] + I[25]*K[25] + I[26]*K[26]);
            }
          } break;
          case 2 : {
            cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
              cimg_forC(res,c) {
              cimg_abort_test;
              const CImg<T> img = get_shared_channel(c%_spectrum);
              const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
              CImg<T> I(8);
              Ttfloat *ptrd = res.data(0,0,0,c);
              if (is_normalized) {
                const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                cimg_for2x2x2(img,x,y,z,0,I,T) {
                  const Ttfloat N = M*(I[0]*I[0] + I[1]*I[1] +
                                       I[2]*I[2] + I[3]*I[3] +
                                       I[4]*I[4] + I[5]*I[5] +
                                       I[6]*I[6] + I[7]*I[7]);
                  *(ptrd++) = (Ttfloat)(N?(I[0]*K[0] + I[1]*K[1] +
                                           I[2]*K[2] + I[3]*K[3] +
                                           I[4]*K[4] + I[5]*K[5] +
                                           I[6]*K[6] + I[7]*K[7])/std::sqrt(N):0);
                }
              } else cimg_for2x2x2(img,x,y,z,0,I,T)
                       *(ptrd++) = (Ttfloat)(I[0]*K[0] + I[1]*K[1] +
                                             I[2]*K[2] + I[3]*K[3] +
                                             I[4]*K[4] + I[5]*K[5] +
                                             I[6]*K[6] + I[7]*K[7]);
            }
          } break;
          default :
          case 1 :
            switch (_kernel._width) {
            case 6 : {
              cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
                cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> img = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> I(36);
                Ttfloat *ptrd = res.data(0,0,0,c);
                if (is_normalized) {
                  const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                  cimg_forZ(img,z) cimg_for6x6(img,x,y,z,0,I,T) {
                    const Ttfloat N = M*(I[ 0]*I[ 0] + I[ 1]*I[ 1] + I[ 2]*I[ 2] + I[ 3]*I[ 3] + I[ 4]*I[ 4] +
                                         I[ 5]*I[ 5] + I[ 6]*I[ 6] + I[ 7]*I[ 7] + I[ 8]*I[ 8] + I[ 9]*I[ 9] +
                                         I[10]*I[10] + I[11]*I[11] + I[12]*I[12] + I[13]*I[13] + I[14]*I[14] +
                                         I[15]*I[15] + I[16]*I[16] + I[17]*I[17] + I[18]*I[18] + I[19]*I[19] +
                                         I[20]*I[20] + I[21]*I[21] + I[22]*I[22] + I[23]*I[23] + I[24]*I[24] +
                                         I[25]*I[25] + I[26]*I[26] + I[27]*I[27] + I[28]*I[28] + I[29]*I[29] +
                                         I[30]*I[30] + I[31]*I[31] + I[32]*I[32] + I[33]*I[33] + I[34]*I[34] +
                                         I[35]*I[35]);
                    *(ptrd++) = (Ttfloat)(N?(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] + I[ 3]*K[ 3] +
                                             I[ 4]*K[ 4] + I[ 5]*K[ 5] + I[ 6]*K[ 6] + I[ 7]*K[ 7] +
                                             I[ 8]*K[ 8] + I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                             I[12]*K[12] + I[13]*K[13] + I[14]*K[14] + I[15]*K[15] +
                                             I[16]*K[16] + I[17]*K[17] + I[18]*K[18] + I[19]*K[19] +
                                             I[20]*K[20] + I[21]*K[21] + I[22]*K[22] + I[23]*K[23] +
                                             I[24]*K[24] + I[25]*K[25] + I[26]*K[26] + I[27]*K[27] +
                                             I[28]*K[28] + I[29]*K[29] + I[30]*K[30] + I[31]*K[31] +
                                             I[32]*K[32] + I[33]*K[33] + I[34]*K[34] + I[35]*K[35])/
                                          std::sqrt(N):0);
                  }
                } else cimg_forZ(img,z) cimg_for6x6(img,x,y,z,0,I,T)
                         *(ptrd++) = (Ttfloat)(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] + I[ 3]*K[ 3] +
                                               I[ 4]*K[ 4] + I[ 5]*K[ 5] + I[ 6]*K[ 6] + I[ 7]*K[ 7] +
                                               I[ 8]*K[ 8] + I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                               I[12]*K[12] + I[13]*K[13] + I[14]*K[14] + I[15]*K[15] +
                                               I[16]*K[16] + I[17]*K[17] + I[18]*K[18] + I[19]*K[19] +
                                               I[20]*K[20] + I[21]*K[21] + I[22]*K[22] + I[23]*K[23] +
                                               I[24]*K[24] + I[25]*K[25] + I[26]*K[26] + I[27]*K[27] +
                                               I[28]*K[28] + I[29]*K[29] + I[30]*K[30] + I[31]*K[31] +
                                               I[32]*K[32] + I[33]*K[33] + I[34]*K[34] + I[35]*K[35]);
              }
            } break;
            case 5 : {
              cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
                cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> img = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> I(25);
                Ttfloat *ptrd = res.data(0,0,0,c);
                if (is_normalized) {
                  const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                  cimg_forZ(img,z) cimg_for5x5(img,x,y,z,0,I,T) {
                    const Ttfloat N = M*(I[ 0]*I[ 0] + I[ 1]*I[ 1] + I[ 2]*I[ 2] + I[ 3]*I[ 3] + I[ 4]*I[ 4] +
                                         I[ 5]*I[ 5] + I[ 6]*I[ 6] + I[ 7]*I[ 7] + I[ 8]*I[ 8] + I[ 9]*I[ 9] +
                                         I[10]*I[10] + I[11]*I[11] + I[12]*I[12] + I[13]*I[13] + I[14]*I[14] +
                                         I[15]*I[15] + I[16]*I[16] + I[17]*I[17] + I[18]*I[18] + I[19]*I[19] +
                                         I[20]*I[20] + I[21]*I[21] + I[22]*I[22] + I[23]*I[23] + I[24]*I[24]);
                    *(ptrd++) = (Ttfloat)(N?(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] + I[ 3]*K[ 3] +
                                             I[ 4]*K[ 4] + I[ 5]*K[ 5] + I[ 6]*K[ 6] + I[ 7]*K[ 7] +
                                             I[ 8]*K[ 8] + I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                             I[12]*K[12] + I[13]*K[13] + I[14]*K[14] + I[15]*K[15] +
                                             I[16]*K[16] + I[17]*K[17] + I[18]*K[18] + I[19]*K[19] +
                                             I[20]*K[20] + I[21]*K[21] + I[22]*K[22] + I[23]*K[23] +
                                             I[24]*K[24])/std::sqrt(N):0);
                  }
                } else cimg_forZ(img,z) cimg_for5x5(img,x,y,z,0,I,T)
                         *(ptrd++) = (Ttfloat)(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] + I[ 3]*K[ 3] +
                                               I[ 4]*K[ 4] + I[ 5]*K[ 5] + I[ 6]*K[ 6] + I[ 7]*K[ 7] +
                                               I[ 8]*K[ 8] + I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                               I[12]*K[12] + I[13]*K[13] + I[14]*K[14] + I[15]*K[15] +
                                               I[16]*K[16] + I[17]*K[17] + I[18]*K[18] + I[19]*K[19] +
                                               I[20]*K[20] + I[21]*K[21] + I[22]*K[22] + I[23]*K[23] +
                                               I[24]*K[24]);
              }
            } break;
            case 4 : {
              cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
                cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> img = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> I(16);
                Ttfloat *ptrd = res.data(0,0,0,c);
                if (is_normalized) {
                  const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                  cimg_forZ(img,z) cimg_for4x4(img,x,y,z,0,I,T) {
                    const Ttfloat N = M*(I[ 0]*I[ 0] + I[ 1]*I[ 1] + I[ 2]*I[ 2] + I[ 3]*I[ 3] +
                                         I[ 4]*I[ 4] + I[ 5]*I[ 5] + I[ 6]*I[ 6] + I[ 7]*I[ 7] +
                                         I[ 8]*I[ 8] + I[ 9]*I[ 9] + I[10]*I[10] + I[11]*I[11] +
                                         I[12]*I[12] + I[13]*I[13] + I[14]*I[14] + I[15]*I[15]);
                    *(ptrd++) = (Ttfloat)(N?(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] + I[ 3]*K[ 3] +
                                             I[ 4]*K[ 4] + I[ 5]*K[ 5] + I[ 6]*K[ 6] + I[ 7]*K[ 7] +
                                             I[ 8]*K[ 8] + I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                             I[12]*K[12] + I[13]*K[13] + I[14]*K[14] + I[15]*K[15])/
                                          std::sqrt(N):0);
                  }
                } else cimg_forZ(img,z) cimg_for4x4(img,x,y,z,0,I,T)
                         *(ptrd++) = (Ttfloat)(I[ 0]*K[ 0] + I[ 1]*K[ 1] + I[ 2]*K[ 2] + I[ 3]*K[ 3] +
                                               I[ 4]*K[ 4] + I[ 5]*K[ 5] + I[ 6]*K[ 6] + I[ 7]*K[ 7] +
                                               I[ 8]*K[ 8] + I[ 9]*K[ 9] + I[10]*K[10] + I[11]*K[11] +
                                               I[12]*K[12] + I[13]*K[13] + I[14]*K[14] + I[15]*K[15]);
              }
            } break;
            case 3 : {
              cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
                cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> img = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> I(9);
                Ttfloat *ptrd = res.data(0,0,0,c);
                if (is_normalized) {
                  const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                  cimg_forZ(img,z) cimg_for3x3(img,x,y,z,0,I,T) {
                    const Ttfloat N = M*(I[0]*I[0] + I[1]*I[1] + I[2]*I[2] +
                                         I[3]*I[3] + I[4]*I[4] + I[5]*I[5] +
                                         I[6]*I[6] + I[7]*I[7] + I[8]*I[8]);
                    *(ptrd++) = (Ttfloat)(N?(I[0]*K[0] + I[1]*K[1] + I[2]*K[2] +
                                             I[3]*K[3] + I[4]*K[4] + I[5]*K[5] +
                                             I[6]*K[6] + I[7]*K[7] + I[8]*K[8])/std::sqrt(N):0);
                  }
                } else cimg_forZ(img,z) cimg_for3x3(img,x,y,z,0,I,T)
                         *(ptrd++) = (Ttfloat)(I[0]*K[0] + I[1]*K[1] + I[2]*K[2] +
                                               I[3]*K[3] + I[4]*K[4] + I[5]*K[5] +
                                               I[6]*K[6] + I[7]*K[7] + I[8]*K[8]);
              }
            } break;
            case 2 : {
              cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
                cimg_forC(res,c) {
                cimg_abort_test;
                const CImg<T> img = get_shared_channel(c%_spectrum);
                const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                CImg<T> I(4);
                Ttfloat *ptrd = res.data(0,0,0,c);
                if (is_normalized) {
                  const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
                  cimg_forZ(img,z) cimg_for2x2(img,x,y,z,0,I,T) {
                    const Ttfloat N = M*(I[0]*I[0] + I[1]*I[1] +
                                         I[2]*I[2] + I[3]*I[3]);
                    *(ptrd++) = (Ttfloat)(N?(I[0]*K[0] + I[1]*K[1] +
                                             I[2]*K[2] + I[3]*K[3])/std::sqrt(N):0);
                  }
                } else cimg_forZ(img,z) cimg_for2x2(img,x,y,z,0,I,T)
                         *(ptrd++) = (Ttfloat)(I[0]*K[0] + I[1]*K[1] +
                                               I[2]*K[2] + I[3]*K[3]);
              }
            } break;
            case 1 :
              if (is_normalized) res.fill(1);
              else cimg_forC(res,c) {
                  cimg_abort_test;
                  const CImg<T> img = get_shared_channel(c%_spectrum);
                  const CImg<t> K = _kernel.get_shared_channel(c%kernel._spectrum);
                  res.get_shared_channel(c).assign(img)*=K[0];
                }
              break;
            }
          }
        }
      }

      if (!res) { // Generic version for other kernels and boundary conditions.
        res.assign(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
        int
          mx2 = kernel.width()/2, my2 = kernel.height()/2, mz2 = kernel.depth()/2,
          mx1 = kernel.width() - mx2 - 1, my1 = kernel.height() - my2 - 1, mz1 = kernel.depth() - mz2 - 1;
        if (is_convolution) cimg::swap(mx1,mx2,my1,my2,mz1,mz2); // Shift kernel center in case of convolution
        const int
          mxe = width() - mx2, mye = height() - my2, mze = depth() - mz2;
        cimg_pragma_openmp(parallel for cimg_openmp_if(!is_inner_parallel && is_outer_parallel))
        cimg_forC(res,c) _cimg_abort_try_omp {
          cimg_abort_test;
          const CImg<T> img = get_shared_channel(c%_spectrum);
          const CImg<t> K = kernel.get_shared_channel(c%kernel._spectrum);
          if (is_normalized) { // Normalized correlation.
            const Ttfloat _M = (Ttfloat)K.magnitude(2), M = _M*_M;
            cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(is_inner_parallel))
            for (int z = mz1; z<mze; ++z)
              for (int y = my1; y<mye; ++y)
                for (int x = mx1; x<mxe; ++x) _cimg_abort_try_omp2 {
                  cimg_abort_test2;
                  Ttfloat val = 0, N = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm) {
                        const Ttfloat _val = (Ttfloat)img(x + xm,y + ym,z + zm);
                        val+=_val*K(mx1 + xm,my1 + ym,mz1 + zm);
                        N+=_val*_val;
                      }
                  N*=M;
                  res(x,y,z,c) = (Ttfloat)(N?val/std::sqrt(N):0);
                } _cimg_abort_catch_omp2
            if (boundary_conditions)
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0, N = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm) {
                        const Ttfloat _val = (Ttfloat)img._atXYZ(x + xm,y + ym,z + zm);
                        val+=_val*K(mx1 + xm,my1 + ym,mz1 + zm);
                        N+=_val*_val;
                      }
                  N*=M;
                  res(x,y,z,c) = (Ttfloat)(N?val/std::sqrt(N):0);
                }
              } _cimg_abort_catch_omp2
            else
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0, N = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm) {
                        const Ttfloat _val = (Ttfloat)img.atXYZ(x + xm,y + ym,z + zm,0,(T)0);
                        val+=_val*K(mx1 + xm,my1 + ym,mz1 + zm);
                        N+=_val*_val;
                      }
                  N*=M;
                  res(x,y,z,c) = (Ttfloat)(N?val/std::sqrt(N):0);
                }
              } _cimg_abort_catch_omp2
          } else { // Classical correlation.
            cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(is_inner_parallel))
              for (int z = mz1; z<mze; ++z)
              for (int y = my1; y<mye; ++y)
                for (int x = mx1; x<mxe; ++x) _cimg_abort_try_omp2 {
                  cimg_abort_test2;
                  Ttfloat val = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm)
                        val+=img(x + xm,y + ym,z + zm)*K(mx1 + xm,my1 + ym,mz1 + zm);
                  res(x,y,z,c) = (Ttfloat)val;
                } _cimg_abort_catch_omp2
            if (boundary_conditions)
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm)
                        val+=img._atXYZ(x + xm,y + ym,z + zm)*K(mx1 + xm,my1 + ym,mz1 + zm);
                  res(x,y,z,c) = (Ttfloat)val;
                }
              } _cimg_abort_catch_omp2
            else
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(is_inner_parallel))
              cimg_forYZ(res,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                for (int x = 0; x<width();
                     (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
                  Ttfloat val = 0;
                  for (int zm = -mz1; zm<=mz2; ++zm)
                    for (int ym = -my1; ym<=my2; ++ym)
                      for (int xm = -mx1; xm<=mx2; ++xm)
                        val+=img.atXYZ(x + xm,y + ym,z + zm,0,(T)0)*K(mx1 + xm,my1 + ym,mz1 + zm);
                  res(x,y,z,c) = (Ttfloat)val;
                }
              } _cimg_abort_catch_omp2
          }
        } _cimg_abort_catch_omp
      }
      cimg_abort_test;
      return res;