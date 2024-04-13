    **/
    CImg<T>& diffusion_tensors(const float sharpness=0.7f, const float anisotropy=0.6f,
                               const float alpha=0.6f, const float sigma=1.1f, const bool is_sqrt=false) {
      CImg<Tfloat> res;
      const float
        nsharpness = std::max(sharpness,1e-5f),
        power1 = (is_sqrt?0.5f:1)*nsharpness,
        power2 = power1/(1e-7f + 1 - anisotropy);
      blur(alpha).normalize(0,(T)255);

      if (_depth>1) { // 3d
        get_structure_tensors().move_to(res).blur(sigma);
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=256 && _height*_depth>=256))
        cimg_forYZ(*this,y,z) {
          Tfloat
            *ptrd0 = res.data(0,y,z,0), *ptrd1 = res.data(0,y,z,1), *ptrd2 = res.data(0,y,z,2),
            *ptrd3 = res.data(0,y,z,3), *ptrd4 = res.data(0,y,z,4), *ptrd5 = res.data(0,y,z,5);
          CImg<floatT> val(3), vec(3,3);
          cimg_forX(*this,x) {
            res.get_tensor_at(x,y,z).symmetric_eigen(val,vec);
            const float
              _l1 = val[2], _l2 = val[1], _l3 = val[0],
              l1 = _l1>0?_l1:0, l2 = _l2>0?_l2:0, l3 = _l3>0?_l3:0,
              ux = vec(0,0), uy = vec(0,1), uz = vec(0,2),
              vx = vec(1,0), vy = vec(1,1), vz = vec(1,2),
              wx = vec(2,0), wy = vec(2,1), wz = vec(2,2),
              n1 = (float)std::pow(1 + l1 + l2 + l3,-power1),
              n2 = (float)std::pow(1 + l1 + l2 + l3,-power2);
            *(ptrd0++) = n1*(ux*ux + vx*vx) + n2*wx*wx;
            *(ptrd1++) = n1*(ux*uy + vx*vy) + n2*wx*wy;
            *(ptrd2++) = n1*(ux*uz + vx*vz) + n2*wx*wz;
            *(ptrd3++) = n1*(uy*uy + vy*vy) + n2*wy*wy;
            *(ptrd4++) = n1*(uy*uz + vy*vz) + n2*wy*wz;
            *(ptrd5++) = n1*(uz*uz + vz*vz) + n2*wz*wz;
          }
        }
      } else { // for 2d images
        get_structure_tensors().move_to(res).blur(sigma);
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=256 && _height>=256))
        cimg_forY(*this,y) {
          Tfloat *ptrd0 = res.data(0,y,0,0), *ptrd1 = res.data(0,y,0,1), *ptrd2 = res.data(0,y,0,2);
          CImg<floatT> val(2), vec(2,2);
          cimg_forX(*this,x) {
            res.get_tensor_at(x,y).symmetric_eigen(val,vec);
            const float
              _l1 = val[1], _l2 = val[0],
              l1 = _l1>0?_l1:0, l2 = _l2>0?_l2:0,
              ux = vec(1,0), uy = vec(1,1),
              vx = vec(0,0), vy = vec(0,1),
              n1 = (float)std::pow(1 + l1 + l2,-power1),
              n2 = (float)std::pow(1 + l1 + l2,-power2);
            *(ptrd0++) = n1*ux*ux + n2*vx*vx;
            *(ptrd1++) = n1*ux*uy + n2*vx*vy;
            *(ptrd2++) = n1*uy*uy + n2*vy*vy;
          }
        }
      }
      return res.move_to(*this);