    **/
    CImg<T>& sharpen(const float amplitude, const bool sharpen_type=false, const float edge=1,
                     const float alpha=0, const float sigma=0) {
      if (is_empty()) return *this;
      T val_min, val_max = max_min(val_min);
      const float nedge = edge/2;
      CImg<Tfloat> velocity(_width,_height,_depth,_spectrum), _veloc_max(_spectrum);

      if (_depth>1) { // 3d
        if (sharpen_type) { // Shock filters.
          CImg<Tfloat> G = (alpha>0?get_blur(alpha).get_structure_tensors():get_structure_tensors());
          if (sigma>0) G.blur(sigma);
          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=32 && _height*_depth>=16))
          cimg_forYZ(G,y,z) {
            Tfloat *ptrG0 = G.data(0,y,z,0), *ptrG1 = G.data(0,y,z,1),
              *ptrG2 = G.data(0,y,z,2), *ptrG3 = G.data(0,y,z,3);
            CImg<Tfloat> val, vec;
            cimg_forX(G,x) {
              G.get_tensor_at(x,y,z).symmetric_eigen(val,vec);
              if (val[0]<0) val[0] = 0;
              if (val[1]<0) val[1] = 0;
              if (val[2]<0) val[2] = 0;
              *(ptrG0++) = vec(0,0);
              *(ptrG1++) = vec(0,1);
              *(ptrG2++) = vec(0,2);
              *(ptrG3++) = 1 - (Tfloat)std::pow(1 + val[0] + val[1] + val[2],-(Tfloat)nedge);
            }
          }
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=512 && _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat
                u = G(x,y,z,0),
                v = G(x,y,z,1),
                w = G(x,y,z,2),
                amp = G(x,y,z,3),
                ixx = Incc + Ipcc - 2*Iccc,
                ixy = (Innc + Ippc - Inpc - Ipnc)/4,
                ixz = (Incn + Ipcp - Incp - Ipcn)/4,
                iyy = Icnc + Icpc - 2*Iccc,
                iyz = (Icnn + Icpp - Icnp - Icpn)/4,
                izz = Iccn + Iccp - 2*Iccc,
                ixf = Incc - Iccc,
                ixb = Iccc - Ipcc,
                iyf = Icnc - Iccc,
                iyb = Iccc - Icpc,
                izf = Iccn - Iccc,
                izb = Iccc - Iccp,
                itt = u*u*ixx + v*v*iyy + w*w*izz + 2*u*v*ixy + 2*u*w*ixz + 2*v*w*iyz,
                it = u*cimg::minmod(ixf,ixb) + v*cimg::minmod(iyf,iyb) + w*cimg::minmod(izf,izb),
                veloc = -amp*cimg::sign(itt)*cimg::abs(it);
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
        } else  // Inverse diffusion.
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat veloc = -Ipcc - Incc - Icpc - Icnc - Iccp - Iccn + 6*Iccc;
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
      } else { // 2d.
        if (sharpen_type) { // Shock filters.
          CImg<Tfloat> G = (alpha>0?get_blur(alpha).get_structure_tensors():get_structure_tensors());
          if (sigma>0) G.blur(sigma);
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=32 && _height>=16))
          cimg_forY(G,y) {
            CImg<Tfloat> val, vec;
            Tfloat *ptrG0 = G.data(0,y,0,0), *ptrG1 = G.data(0,y,0,1), *ptrG2 = G.data(0,y,0,2);
            cimg_forX(G,x) {
              G.get_tensor_at(x,y).symmetric_eigen(val,vec);
              if (val[0]<0) val[0] = 0;
              if (val[1]<0) val[1] = 0;
              *(ptrG0++) = vec(0,0);
              *(ptrG1++) = vec(0,1);
              *(ptrG2++) = 1 - (Tfloat)std::pow(1 + val[0] + val[1],-(Tfloat)nedge);
            }
          }
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=512 && _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat
                u = G(x,y,0),
                v = G(x,y,1),
                amp = G(x,y,2),
                ixx = Inc + Ipc - 2*Icc,
                ixy = (Inn + Ipp - Inp - Ipn)/4,
                iyy = Icn + Icp - 2*Icc,
                ixf = Inc - Icc,
                ixb = Icc - Ipc,
                iyf = Icn - Icc,
                iyb = Icc - Icp,
                itt = u*u*ixx + v*v*iyy + 2*u*v*ixy,
                it = u*cimg::minmod(ixf,ixb) + v*cimg::minmod(iyf,iyb),
                veloc = -amp*cimg::sign(itt)*cimg::abs(it);
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
        } else // Inverse diffusion.
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat veloc = -Ipc - Inc - Icp - Icn + 4*Icc;
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
      }
      const Tfloat veloc_max = _veloc_max.max();
      if (veloc_max<=0) return *this;
      return ((velocity*=amplitude/veloc_max)+=*this).cut(val_min,val_max).move_to(*this);