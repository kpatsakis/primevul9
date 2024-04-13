    **/
    CImg<T>& distance_eikonal(const unsigned int nb_iterations, const float band_size=0, const float time_step=0.5f) {
      if (is_empty()) return *this;
      CImg<Tfloat> velocity(*this,false);
      for (unsigned int iteration = 0; iteration<nb_iterations; ++iteration) {
        Tfloat *ptrd = velocity._data, veloc_max = 0;
        if (_depth>1) { // 3d
          CImg_3x3x3(I,Tfloat);
          cimg_forC(*this,c) cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) if (band_size<=0 || cimg::abs(Iccc)<band_size) {
            const Tfloat
              gx = (Incc - Ipcc)/2,
              gy = (Icnc - Icpc)/2,
              gz = (Iccn - Iccp)/2,
              sgn = -cimg::sign(Iccc),
              ix = gx*sgn>0?(Incc - Iccc):(Iccc - Ipcc),
              iy = gy*sgn>0?(Icnc - Iccc):(Iccc - Icpc),
              iz = gz*sgn>0?(Iccn - Iccc):(Iccc - Iccp),
              ng = 1e-5f + cimg::hypot(gx,gy,gz),
              ngx = gx/ng,
              ngy = gy/ng,
              ngz = gz/ng,
              veloc = sgn*(ngx*ix + ngy*iy + ngz*iz - 1);
            *(ptrd++) = veloc;
            if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
          } else *(ptrd++) = 0;
        } else { // 2d version
          CImg_3x3(I,Tfloat);
          cimg_forC(*this,c) cimg_for3x3(*this,x,y,0,c,I,Tfloat) if (band_size<=0 || cimg::abs(Icc)<band_size) {
            const Tfloat
              gx = (Inc - Ipc)/2,
              gy = (Icn - Icp)/2,
              sgn = -cimg::sign(Icc),
              ix = gx*sgn>0?(Inc - Icc):(Icc - Ipc),
              iy = gy*sgn>0?(Icn - Icc):(Icc - Icp),
              ng = std::max((Tfloat)1e-5,cimg::hypot(gx,gy)),
              ngx = gx/ng,
              ngy = gy/ng,
              veloc = sgn*(ngx*ix + ngy*iy - 1);
            *(ptrd++) = veloc;
            if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
          } else *(ptrd++) = 0;
        }
        if (veloc_max>0) *this+=(velocity*=time_step/veloc_max);
      }
      return *this;