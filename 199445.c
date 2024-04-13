    //! Blur image with the median filter \newinstance.
    CImg<T> get_blur_median(const unsigned int n, const float threshold=0) const {
      if (is_empty() || n<=1) return +*this;
      CImg<T> res(_width,_height,_depth,_spectrum);
      T *ptrd = res._data;
      cimg::unused(ptrd);
      const int hr = (int)n/2, hl = n - hr - 1;
      if (res._depth!=1) { // 3d
        if (threshold>0)
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=16 && _height*_depth*_spectrum>=4))
          cimg_forXYZC(*this,x,y,z,c) { // With threshold.
            const int
              x0 = x - hl, y0 = y - hl, z0 = z - hl, x1 = x + hr, y1 = y + hr, z1 = z + hr,
              nx0 = x0<0?0:x0, ny0 = y0<0?0:y0, nz0 = z0<0?0:z0,
              nx1 = x1>=width()?width() - 1:x1, ny1 = y1>=height()?height() - 1:y1, nz1 = z1>=depth()?depth() - 1:z1;
            const Tfloat val0 = (Tfloat)(*this)(x,y,z,c);
            CImg<T> values(n*n*n);
            unsigned int nb_values = 0;
            T *ptrd = values.data();
            cimg_for_inXYZ(*this,nx0,ny0,nz0,nx1,ny1,nz1,p,q,r)
              if (cimg::abs((*this)(p,q,r,c) - val0)<=threshold) { *(ptrd++) = (*this)(p,q,r,c); ++nb_values; }
            res(x,y,z,c) = nb_values?values.get_shared_points(0,nb_values - 1).median():(*this)(x,y,z,c);
          }
        else
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=16 && _height*_depth*_spectrum>=4))
          cimg_forXYZC(*this,x,y,z,c) { // Without threshold.
            const int
              x0 = x - hl, y0 = y - hl, z0 = z - hl, x1 = x + hr, y1 = y + hr, z1 = z + hr,
              nx0 = x0<0?0:x0, ny0 = y0<0?0:y0, nz0 = z0<0?0:z0,
              nx1 = x1>=width()?width() - 1:x1, ny1 = y1>=height()?height() - 1:y1, nz1 = z1>=depth()?depth() - 1:z1;
            res(x,y,z,c) = get_crop(nx0,ny0,nz0,c,nx1,ny1,nz1,c).median();
          }
      } else {
        if (threshold>0)
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=16 && _height*_spectrum>=4))
          cimg_forXYC(*this,x,y,c) { // With threshold.
            const int
              x0 = x - hl, y0 = y - hl, x1 = x + hr, y1 = y + hr,
              nx0 = x0<0?0:x0, ny0 = y0<0?0:y0,
                                        nx1 = x1>=width()?width() - 1:x1, ny1 = y1>=height()?height() - 1:y1;
            const Tfloat val0 = (Tfloat)(*this)(x,y,c);
            CImg<T> values(n*n);
            unsigned int nb_values = 0;
            T *ptrd = values.data();
            cimg_for_inXY(*this,nx0,ny0,nx1,ny1,p,q)
              if (cimg::abs((*this)(p,q,c) - val0)<=threshold) { *(ptrd++) = (*this)(p,q,c); ++nb_values; }
            res(x,y,c) = nb_values?values.get_shared_points(0,nb_values - 1).median():(*this)(x,y,c);
          }
        else {
          const int
            w1 = width() - 1, h1 = height() - 1,
            w2 = width() - 2, h2 = height() - 2,
            w3 = width() - 3, h3 = height() - 3,
            w4 = width() - 4, h4 = height() - 4;
          switch (n) { // Without threshold.
          case 3 : {
            cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
            cimg_forC(*this,c) {
              CImg<T> I(9);
              cimg_for_in3x3(*this,1,1,w2,h2,x,y,0,c,I,T)
                res(x,y,c) = cimg::median(I[0],I[1],I[2],I[3],I[4],I[5],I[6],I[7],I[8]);
              cimg_for_borderXY(*this,x,y,1)
                res(x,y,c) = get_crop(std::max(0,x - 1),std::max(0,y - 1),0,c,
                                      std::min(w1,x + 1),std::min(h1,y + 1),0,c).median();
            }
          } break;
          case 5 : {
            cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
            cimg_forC(*this,c) {
              CImg<T> I(25);
              cimg_for_in5x5(*this,2,2,w3,h3,x,y,0,c,I,T)
                res(x,y,c) = cimg::median(I[0],I[1],I[2],I[3],I[4],
                                          I[5],I[6],I[7],I[8],I[9],
                                          I[10],I[11],I[12],I[13],I[14],
                                          I[15],I[16],I[17],I[18],I[19],
                                          I[20],I[21],I[22],I[23],I[24]);
              cimg_for_borderXY(*this,x,y,2)
                res(x,y,c) = get_crop(std::max(0,x - 2),std::max(0,y - 2),0,c,
                                      std::min(w1,x + 2),std::min(h1,y + 2),0,c).median();
            }
          } break;
          case 7 : {
            cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
            cimg_forC(*this,c) {
              CImg<T> I(49);
              cimg_for_in7x7(*this,3,3,w4,h4,x,y,0,c,I,T)
                res(x,y,c) = cimg::median(I[0],I[1],I[2],I[3],I[4],I[5],I[6],
                                          I[7],I[8],I[9],I[10],I[11],I[12],I[13],
                                          I[14],I[15],I[16],I[17],I[18],I[19],I[20],
                                          I[21],I[22],I[23],I[24],I[25],I[26],I[27],
                                          I[28],I[29],I[30],I[31],I[32],I[33],I[34],
                                          I[35],I[36],I[37],I[38],I[39],I[40],I[41],
                                          I[42],I[43],I[44],I[45],I[46],I[47],I[48]);
              cimg_for_borderXY(*this,x,y,3)
                res(x,y,c) = get_crop(std::max(0,x - 3),std::max(0,y - 3),0,c,
                                      std::min(w1,x + 3),std::min(h1,y + 3),0,c).median();
            }
          } break;
          default : {
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=16 && _height*_spectrum>=4))
            cimg_forXYC(*this,x,y,c) {
              const int
                x0 = x - hl, y0 = y - hl, x1 = x + hr, y1 = y + hr,
                nx0 = x0<0?0:x0, ny0 = y0<0?0:y0,
                                          nx1 = x1>=width()?width() - 1:x1, ny1 = y1>=height()?height() - 1:y1;
              res(x,y,c) = get_crop(nx0,ny0,0,c,nx1,ny1,0,c).median();
            }
          }
          }
        }
      }
      return res;