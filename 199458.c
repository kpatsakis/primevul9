    //! Estimate displacement field between two images \newinstance.
    CImg<floatT> get_displacement(const CImg<T>& source,
                                  const float smoothness=0.1f, const float precision=5.0f,
                                  const unsigned int nb_scales=0, const unsigned int iteration_max=10000,
                                  const bool is_backward=false,
                                  const CImg<floatT>& guide=CImg<floatT>::const_empty()) const {
      if (is_empty() || !source) return +*this;
      if (!is_sameXYZC(source))
        throw CImgArgumentException(_cimg_instance
                                    "displacement(): Instance and source image (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    source._width,source._height,source._depth,source._spectrum,source._data);
      if (precision<0)
        throw CImgArgumentException(_cimg_instance
                                    "displacement(): Invalid specified precision %g "
                                    "(should be >=0)",
                                    cimg_instance,
                                    precision);

      const bool is_3d = source._depth>1;
      const unsigned int constraint = is_3d?3:2;

      if (guide &&
          (guide._width!=_width || guide._height!=_height || guide._depth!=_depth || guide._spectrum<constraint))
        throw CImgArgumentException(_cimg_instance
                                    "displacement(): Specified guide (%u,%u,%u,%u,%p) "
                                    "has invalid dimensions.",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);

      const unsigned int
        mins = is_3d?cimg::min(_width,_height,_depth):std::min(_width,_height),
        _nb_scales = nb_scales>0?nb_scales:
        (unsigned int)cimg::round(std::log(mins/8.0)/std::log(1.5),1,1);

      const float _precision = (float)std::pow(10.0,-(double)precision);
      float sm, sM = source.max_min(sm), tm, tM = max_min(tm);
      const float sdelta = sm==sM?1:(sM - sm), tdelta = tm==tM?1:(tM - tm);

      CImg<floatT> U, V;
      floatT bound = 0;
      for (int scale = (int)_nb_scales - 1; scale>=0; --scale) {
        const float factor = (float)std::pow(1.5,(double)scale);
        const unsigned int
          _sw = (unsigned int)(_width/factor), sw = _sw?_sw:1,
          _sh = (unsigned int)(_height/factor), sh = _sh?_sh:1,
          _sd = (unsigned int)(_depth/factor), sd = _sd?_sd:1;
        if (sw<5 && sh<5 && (!is_3d || sd<5)) continue;  // skip too small scales.
        const CImg<Tfloat>
          I1 = (source.get_resize(sw,sh,sd,-100,2)-=sm)/=sdelta,
          I2 = (get_resize(I1,2)-=tm)/=tdelta;
        if (guide._spectrum>constraint) guide.get_resize(I2._width,I2._height,I2._depth,-100,1).move_to(V);
        if (U) (U*=1.5f).resize(I2._width,I2._height,I2._depth,-100,3);
        else {
          if (guide)
            guide.get_shared_channels(0,is_3d?2:1).get_resize(I2._width,I2._height,I2._depth,-100,2).move_to(U);
          else U.assign(I2._width,I2._height,I2._depth,is_3d?3:2,0);
        }

        float dt = 2, energy = cimg::type<float>::max();
        const CImgList<Tfloat> dI = is_backward?I1.get_gradient():I2.get_gradient();
        cimg_abort_init;

        for (unsigned int iteration = 0; iteration<iteration_max; ++iteration) {
          cimg_abort_test;
          float _energy = 0;

          if (is_3d) { // 3d version.
            if (smoothness>=0) // Isotropic regularization.
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_height*_depth>=8 && _width>=16)
                                 reduction(+:_energy))
              cimg_forYZ(U,y,z) {
                const int
                  _p1y = y?y - 1:0, _n1y = y<U.height() - 1?y + 1:y,
                  _p1z = z?z - 1:0, _n1z = z<U.depth() - 1?z + 1:z;
                cimg_for3X(U,x) {
                  const float
                    X = is_backward?x - U(x,y,z,0):x + U(x,y,z,0),
                    Y = is_backward?y - U(x,y,z,1):y + U(x,y,z,1),
                    Z = is_backward?z - U(x,y,z,2):z + U(x,y,z,2);
                  float delta_I = 0, _energy_regul = 0;
                  if (is_backward) cimg_forC(I2,c) delta_I+=(float)(I1._linear_atXYZ(X,Y,Z,c) - I2(x,y,z,c));
                  else cimg_forC(I2,c) delta_I+=(float)(I1(x,y,z,c) - I2._linear_atXYZ(X,Y,Z,c));
                  cimg_forC(U,c) {
                    const float
                      Ux = 0.5f*(U(_n1x,y,z,c) - U(_p1x,y,z,c)),
                      Uy = 0.5f*(U(x,_n1y,z,c) - U(x,_p1y,z,c)),
                      Uz = 0.5f*(U(x,y,_n1z,c) - U(x,y,_p1z,c)),
                      Uxx = U(_n1x,y,z,c) + U(_p1x,y,z,c),
                      Uyy = U(x,_n1y,z,c) + U(x,_p1y,z,c),
                      Uzz = U(x,y,_n1z,c) + U(x,y,_p1z,c);
                    U(x,y,z,c) = (float)(U(x,y,z,c) + dt*(delta_I*dI[c]._linear_atXYZ(X,Y,Z) +
                                                          smoothness* ( Uxx + Uyy + Uzz)))/(1 + 6*smoothness*dt);
                    _energy_regul+=Ux*Ux + Uy*Uy + Uz*Uz;
                  }
                  if (is_backward) { // Constraint displacement vectors to stay in image.
                    if (U(x,y,z,0)>x) U(x,y,z,0) = (float)x;
                    if (U(x,y,z,1)>y) U(x,y,z,1) = (float)y;
                    if (U(x,y,z,2)>z) U(x,y,z,2) = (float)z;
                    bound = (float)x - _width; if (U(x,y,z,0)<=bound) U(x,y,z,0) = bound;
                    bound = (float)y - _height; if (U(x,y,z,1)<=bound) U(x,y,z,1) = bound;
                    bound = (float)z - _depth; if (U(x,y,z,2)<=bound) U(x,y,z,2) = bound;
                  } else {
                    if (U(x,y,z,0)<-x) U(x,y,z,0) = -(float)x;
                    if (U(x,y,z,1)<-y) U(x,y,z,1) = -(float)y;
                    if (U(x,y,z,2)<-z) U(x,y,z,2) = -(float)z;
                    bound = (float)_width - x; if (U(x,y,z,0)>=bound) U(x,y,z,0) = bound;
                    bound = (float)_height - y; if (U(x,y,z,1)>=bound) U(x,y,z,1) = bound;
                    bound = (float)_depth - z; if (U(x,y,z,2)>=bound) U(x,y,z,2) = bound;
                  }
                  _energy+=delta_I*delta_I + smoothness*_energy_regul;
                }
                if (V) cimg_forXYZ(V,x,y,z) if (V(x,y,z,3)) { // Apply constraints.
                    U(x,y,z,0) = V(x,y,z,0)/factor;
                    U(x,y,z,1) = V(x,y,z,1)/factor;
                    U(x,y,z,2) = V(x,y,z,2)/factor;
                  }
              } else { // Anisotropic regularization.
              const float nsmoothness = -smoothness;
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_height*_depth>=8 && _width>=16)
                                 reduction(+:_energy))
              cimg_forYZ(U,y,z) {
                const int
                  _p1y = y?y - 1:0, _n1y = y<U.height() - 1?y + 1:y,
                  _p1z = z?z - 1:0, _n1z = z<U.depth() - 1?z + 1:z;
                cimg_for3X(U,x) {
                  const float
                    X = is_backward?x - U(x,y,z,0):x + U(x,y,z,0),
                    Y = is_backward?y - U(x,y,z,1):y + U(x,y,z,1),
                    Z = is_backward?z - U(x,y,z,2):z + U(x,y,z,2);
                  float delta_I = 0, _energy_regul = 0;
                  if (is_backward) cimg_forC(I2,c) delta_I+=(float)(I1._linear_atXYZ(X,Y,Z,c) - I2(x,y,z,c));
                  else cimg_forC(I2,c) delta_I+=(float)(I1(x,y,z,c) - I2._linear_atXYZ(X,Y,Z,c));
                  cimg_forC(U,c) {
                    const float
                      Ux = 0.5f*(U(_n1x,y,z,c) - U(_p1x,y,z,c)),
                      Uy = 0.5f*(U(x,_n1y,z,c) - U(x,_p1y,z,c)),
                      Uz = 0.5f*(U(x,y,_n1z,c) - U(x,y,_p1z,c)),
                      N2 = Ux*Ux + Uy*Uy + Uz*Uz,
                      N = std::sqrt(N2),
                      N3 = 1e-5f + N2*N,
                      coef_a = (1 - Ux*Ux/N2)/N,
                      coef_b = -2*Ux*Uy/N3,
                      coef_c = -2*Ux*Uz/N3,
                      coef_d = (1 - Uy*Uy/N2)/N,
                      coef_e = -2*Uy*Uz/N3,
                      coef_f = (1 - Uz*Uz/N2)/N,
                      Uxx = U(_n1x,y,z,c) + U(_p1x,y,z,c),
                      Uyy = U(x,_n1y,z,c) + U(x,_p1y,z,c),
                      Uzz = U(x,y,_n1z,c) + U(x,y,_p1z,c),
                      Uxy = 0.25f*(U(_n1x,_n1y,z,c) + U(_p1x,_p1y,z,c) - U(_n1x,_p1y,z,c) - U(_n1x,_p1y,z,c)),
                      Uxz = 0.25f*(U(_n1x,y,_n1z,c) + U(_p1x,y,_p1z,c) - U(_n1x,y,_p1z,c) - U(_n1x,y,_p1z,c)),
                      Uyz = 0.25f*(U(x,_n1y,_n1z,c) + U(x,_p1y,_p1z,c) - U(x,_n1y,_p1z,c) - U(x,_n1y,_p1z,c));
                    U(x,y,z,c) = (float)(U(x,y,z,c) + dt*(delta_I*dI[c]._linear_atXYZ(X,Y,Z) +
                                                          nsmoothness* ( coef_a*Uxx + coef_b*Uxy +
                                                                         coef_c*Uxz + coef_d*Uyy +
                                                                         coef_e*Uyz + coef_f*Uzz ))
                                         )/(1 + 2*(coef_a + coef_d + coef_f)*nsmoothness*dt);
                    _energy_regul+=N;
                  }
                  if (is_backward) { // Constraint displacement vectors to stay in image.
                    if (U(x,y,z,0)>x) U(x,y,z,0) = (float)x;
                    if (U(x,y,z,1)>y) U(x,y,z,1) = (float)y;
                    if (U(x,y,z,2)>z) U(x,y,z,2) = (float)z;
                    bound = (float)x - _width; if (U(x,y,z,0)<=bound) U(x,y,z,0) = bound;
                    bound = (float)y - _height; if (U(x,y,z,1)<=bound) U(x,y,z,1) = bound;
                    bound = (float)z - _depth; if (U(x,y,z,2)<=bound) U(x,y,z,2) = bound;
                  } else {
                    if (U(x,y,z,0)<-x) U(x,y,z,0) = -(float)x;
                    if (U(x,y,z,1)<-y) U(x,y,z,1) = -(float)y;
                    if (U(x,y,z,2)<-z) U(x,y,z,2) = -(float)z;
                    bound = (float)_width - x; if (U(x,y,z,0)>=bound) U(x,y,z,0) = bound;
                    bound = (float)_height - y; if (U(x,y,z,1)>=bound) U(x,y,z,1) = bound;
                    bound = (float)_depth - z; if (U(x,y,z,2)>=bound) U(x,y,z,2) = bound;
                  }
                  _energy+=delta_I*delta_I + nsmoothness*_energy_regul;
                }
                if (V) cimg_forXYZ(V,x,y,z) if (V(x,y,z,3)) { // Apply constraints.
                    U(x,y,z,0) = V(x,y,z,0)/factor;
                    U(x,y,z,1) = V(x,y,z,1)/factor;
                    U(x,y,z,2) = V(x,y,z,2)/factor;
                  }
              }
            }
          } else { // 2d version.
            if (smoothness>=0) // Isotropic regularization.
              cimg_pragma_openmp(parallel for cimg_openmp_if(_height>=8 && _width>=16) reduction(+:_energy))
              cimg_forY(U,y) {
                const int _p1y = y?y - 1:0, _n1y = y<U.height() - 1?y + 1:y;
                cimg_for3X(U,x) {
                  const float
                    X = is_backward?x - U(x,y,0):x + U(x,y,0),
                    Y = is_backward?y - U(x,y,1):y + U(x,y,1);
                  float delta_I = 0, _energy_regul = 0;
                  if (is_backward) cimg_forC(I2,c) delta_I+=(float)(I1._linear_atXY(X,Y,c) - I2(x,y,c));
                  else cimg_forC(I2,c) delta_I+=(float)(I1(x,y,c) - I2._linear_atXY(X,Y,c));
                  cimg_forC(U,c) {
                    const float
                      Ux = 0.5f*(U(_n1x,y,c) - U(_p1x,y,c)),
                      Uy = 0.5f*(U(x,_n1y,c) - U(x,_p1y,c)),
                      Uxx = U(_n1x,y,c) + U(_p1x,y,c),
                      Uyy = U(x,_n1y,c) + U(x,_p1y,c);
                    U(x,y,c) = (float)(U(x,y,c) + dt*(delta_I*dI[c]._linear_atXY(X,Y) +
                                                      smoothness*( Uxx + Uyy )))/(1 + 4*smoothness*dt);
                    _energy_regul+=Ux*Ux + Uy*Uy;
                  }
                  if (is_backward) { // Constraint displacement vectors to stay in image.
                    if (U(x,y,0)>x) U(x,y,0) = (float)x;
                    if (U(x,y,1)>y) U(x,y,1) = (float)y;
                    bound = (float)x - _width; if (U(x,y,0)<=bound) U(x,y,0) = bound;
                    bound = (float)y - _height; if (U(x,y,1)<=bound) U(x,y,1) = bound;
                  } else {
                    if (U(x,y,0)<-x) U(x,y,0) = -(float)x;
                    if (U(x,y,1)<-y) U(x,y,1) = -(float)y;
                    bound = (float)_width - x; if (U(x,y,0)>=bound) U(x,y,0) = bound;
                    bound = (float)_height - y; if (U(x,y,1)>=bound) U(x,y,1) = bound;
                  }
                  _energy+=delta_I*delta_I + smoothness*_energy_regul;
                }
                if (V) cimg_forX(V,x) if (V(x,y,2)) { // Apply constraints.
                    U(x,y,0) = V(x,y,0)/factor;
                    U(x,y,1) = V(x,y,1)/factor;
                  }
              } else { // Anisotropic regularization.
              const float nsmoothness = -smoothness;
              cimg_pragma_openmp(parallel for cimg_openmp_if(_height>=8 && _width>=16) reduction(+:_energy))
              cimg_forY(U,y) {
                const int _p1y = y?y - 1:0, _n1y = y<U.height() - 1?y + 1:y;
                cimg_for3X(U,x) {
                  const float
                    X = is_backward?x - U(x,y,0):x + U(x,y,0),
                    Y = is_backward?y - U(x,y,1):y + U(x,y,1);
                  float delta_I = 0, _energy_regul = 0;
                  if (is_backward) cimg_forC(I2,c) delta_I+=(float)(I1._linear_atXY(X,Y,c) - I2(x,y,c));
                  else cimg_forC(I2,c) delta_I+=(float)(I1(x,y,c) - I2._linear_atXY(X,Y,c));
                  cimg_forC(U,c) {
                    const float
                      Ux = 0.5f*(U(_n1x,y,c) - U(_p1x,y,c)),
                      Uy = 0.5f*(U(x,_n1y,c) - U(x,_p1y,c)),
                      N2 = Ux*Ux + Uy*Uy,
                      N = std::sqrt(N2),
                      N3 = 1e-5f + N2*N,
                      coef_a = Uy*Uy/N3,
                      coef_b = -2*Ux*Uy/N3,
                      coef_c = Ux*Ux/N3,
                      Uxx = U(_n1x,y,c) + U(_p1x,y,c),
                      Uyy = U(x,_n1y,c) + U(x,_p1y,c),
                      Uxy = 0.25f*(U(_n1x,_n1y,c) + U(_p1x,_p1y,c) - U(_n1x,_p1y,c) - U(_n1x,_p1y,c));
                    U(x,y,c) = (float)(U(x,y,c) + dt*(delta_I*dI[c]._linear_atXY(X,Y) +
                                                      nsmoothness*( coef_a*Uxx + coef_b*Uxy + coef_c*Uyy )))/
                      (1 + 2*(coef_a + coef_c)*nsmoothness*dt);
                    _energy_regul+=N;
                  }
                  if (is_backward) { // Constraint displacement vectors to stay in image.
                    if (U(x,y,0)>x) U(x,y,0) = (float)x;
                    if (U(x,y,1)>y) U(x,y,1) = (float)y;
                    bound = (float)x - _width; if (U(x,y,0)<=bound) U(x,y,0) = bound;
                    bound = (float)y - _height; if (U(x,y,1)<=bound) U(x,y,1) = bound;
                  } else {
                    if (U(x,y,0)<-x) U(x,y,0) = -(float)x;
                    if (U(x,y,1)<-y) U(x,y,1) = -(float)y;
                    bound = (float)_width - x; if (U(x,y,0)>=bound) U(x,y,0) = bound;
                    bound = (float)_height - y; if (U(x,y,1)>=bound) U(x,y,1) = bound;
                  }
                  _energy+=delta_I*delta_I + nsmoothness*_energy_regul;
                }
                if (V) cimg_forX(V,x) if (V(x,y,2)) { // Apply constraints.
                    U(x,y,0) = V(x,y,0)/factor;
                    U(x,y,1) = V(x,y,1)/factor;
                  }
              }
            }
          }
          const float d_energy = (_energy - energy)/(sw*sh*sd);
          if (d_energy<=0 && -d_energy<_precision) break;
          if (d_energy>0) dt*=0.5f;
          energy = _energy;
        }
      }
      return U;