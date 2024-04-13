    template<typename t>
    CImg<T>& blur_anisotropic(const CImg<t>& G,
                              const float amplitude=60, const float dl=0.8f, const float da=30,
                              const float gauss_prec=2, const unsigned int interpolation_type=0,
                              const bool is_fast_approx=1) {

      // Check arguments and init variables
      if (!is_sameXYZ(G) || (G._spectrum!=3 && G._spectrum!=6))
        throw CImgArgumentException(_cimg_instance
                                    "blur_anisotropic(): Invalid specified diffusion tensor field (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    G._width,G._height,G._depth,G._spectrum,G._data);

      if (is_empty() || amplitude<=0 || dl<0) return *this;
      const bool is_3d = (G._spectrum==6);
      T val_min, val_max = max_min(val_min);
      _cimg_abort_init_omp;
      cimg_abort_init;

      if (da<=0) {  // Iterated oriented Laplacians
        CImg<Tfloat> velocity(_width,_height,_depth,_spectrum);
        for (unsigned int iteration = 0; iteration<(unsigned int)amplitude; ++iteration) {
          Tfloat *ptrd = velocity._data, veloc_max = 0;
          if (is_3d) // 3d version
            cimg_forC(*this,c) {
              cimg_abort_test;
              CImg_3x3x3(I,Tfloat);
              cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
                const Tfloat
                  ixx = Incc + Ipcc - 2*Iccc,
                  ixy = (Innc + Ippc - Inpc - Ipnc)/4,
                  ixz = (Incn + Ipcp - Incp - Ipcn)/4,
                  iyy = Icnc + Icpc - 2*Iccc,
                  iyz = (Icnn + Icpp - Icnp - Icpn)/4,
                  izz = Iccn + Iccp - 2*Iccc,
                  veloc = (Tfloat)(G(x,y,z,0)*ixx + 2*G(x,y,z,1)*ixy + 2*G(x,y,z,2)*ixz +
                                   G(x,y,z,3)*iyy + 2*G(x,y,z,4)*iyz + G(x,y,z,5)*izz);
                *(ptrd++) = veloc;
                if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
              }
            }
          else // 2d version
            cimg_forZC(*this,z,c) {
              cimg_abort_test;
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
                const Tfloat
                  ixx = Inc + Ipc - 2*Icc,
                  ixy = (Inn + Ipp - Inp - Ipn)/4,
                  iyy = Icn + Icp - 2*Icc,
                  veloc = (Tfloat)(G(x,y,0,0)*ixx + 2*G(x,y,0,1)*ixy + G(x,y,0,2)*iyy);
                *(ptrd++) = veloc;
                if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
              }
            }
          if (veloc_max>0) *this+=(velocity*=dl/veloc_max);
        }
      } else { // LIC-based smoothing.
        const ulongT whd = (ulongT)_width*_height*_depth;
        const float sqrt2amplitude = (float)std::sqrt(2*amplitude);
        const int dx1 = width() - 1, dy1 = height() - 1, dz1 = depth() - 1;
        CImg<Tfloat> res(_width,_height,_depth,_spectrum,0), W(_width,_height,_depth,is_3d?4:3), val(_spectrum,1,1,1,0);
        int N = 0;
        if (is_3d) { // 3d version
          for (float phi = cimg::mod(180.0f,da)/2.0f; phi<=180; phi+=da) {
            const float phir = (float)(phi*cimg::PI/180), datmp = (float)(da/std::cos(phir)),
              da2 = datmp<1?360.0f:datmp;
            for (float theta = 0; theta<360; (theta+=da2),++N) {
              const float
                thetar = (float)(theta*cimg::PI/180),
                vx = (float)(std::cos(thetar)*std::cos(phir)),
                vy = (float)(std::sin(thetar)*std::cos(phir)),
                vz = (float)std::sin(phir);
              const t
                *pa = G.data(0,0,0,0), *pb = G.data(0,0,0,1), *pc = G.data(0,0,0,2),
                *pd = G.data(0,0,0,3), *pe = G.data(0,0,0,4), *pf = G.data(0,0,0,5);
              Tfloat *pd0 = W.data(0,0,0,0), *pd1 = W.data(0,0,0,1), *pd2 = W.data(0,0,0,2), *pd3 = W.data(0,0,0,3);
              cimg_forXYZ(G,xg,yg,zg) {
                const t a = *(pa++), b = *(pb++), c = *(pc++), d = *(pd++), e = *(pe++), f = *(pf++);
                const float
                  u = (float)(a*vx + b*vy + c*vz),
                  v = (float)(b*vx + d*vy + e*vz),
                  w = (float)(c*vx + e*vy + f*vz),
                  n = 1e-5f + cimg::hypot(u,v,w),
                  dln = dl/n;
                *(pd0++) = (Tfloat)(u*dln);
                *(pd1++) = (Tfloat)(v*dln);
                *(pd2++) = (Tfloat)(w*dln);
                *(pd3++) = (Tfloat)n;
              }

              cimg_abort_test;
              cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>=256 && _height*_depth>=2)
                                 firstprivate(val))
              cimg_forYZ(*this,y,z) _cimg_abort_try_omp2 {
                cimg_abort_test2;
                cimg_forX(*this,x) {
                  val.fill(0);
                  const float
                    n = (float)W(x,y,z,3),
                    fsigma = (float)(n*sqrt2amplitude),
                    fsigma2 = 2*fsigma*fsigma,
                    length = gauss_prec*fsigma;
                  float
                    S = 0,
                    X = (float)x,
                    Y = (float)y,
                    Z = (float)z;
                  switch (interpolation_type) {
                  case 0 : { // Nearest neighbor
                    for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1 && Z>=0 && Z<=dz1; l+=dl) {
                      const int
                        cx = (int)(X + 0.5f),
                        cy = (int)(Y + 0.5f),
                        cz = (int)(Z + 0.5f);
                      const float
                        u = (float)W(cx,cy,cz,0),
                        v = (float)W(cx,cy,cz,1),
                        w = (float)W(cx,cy,cz,2);
                      if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)(*this)(cx,cy,cz,c); ++S; }
                      else {
                        const float coef = (float)std::exp(-l*l/fsigma2);
                        cimg_forC(*this,c) val[c]+=(Tfloat)(coef*(*this)(cx,cy,cz,c));
                        S+=coef;
                      }
                      X+=u; Y+=v; Z+=w;
                    }
                  } break;
                  case 1 : { // Linear interpolation
                    for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1 && Z>=0 && Z<=dz1; l+=dl) {
                      const float
                        u = (float)(W._linear_atXYZ(X,Y,Z,0)),
                        v = (float)(W._linear_atXYZ(X,Y,Z,1)),
                        w = (float)(W._linear_atXYZ(X,Y,Z,2));
                      if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXYZ(X,Y,Z,c); ++S; }
                      else {
                        const float coef = (float)std::exp(-l*l/fsigma2);
                        cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXYZ(X,Y,Z,c));
                        S+=coef;
                      }
                      X+=u; Y+=v; Z+=w;
                    }
                  } break;
                  default : { // 2nd order Runge Kutta
                    for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1 && Z>=0 && Z<=dz1; l+=dl) {
                      const float
                        u0 = (float)(0.5f*W._linear_atXYZ(X,Y,Z,0)),
                        v0 = (float)(0.5f*W._linear_atXYZ(X,Y,Z,1)),
                        w0 = (float)(0.5f*W._linear_atXYZ(X,Y,Z,2)),
                        u = (float)(W._linear_atXYZ(X + u0,Y + v0,Z + w0,0)),
                        v = (float)(W._linear_atXYZ(X + u0,Y + v0,Z + w0,1)),
                        w = (float)(W._linear_atXYZ(X + u0,Y + v0,Z + w0,2));
                      if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXYZ(X,Y,Z,c); ++S; }
                      else {
                        const float coef = (float)std::exp(-l*l/fsigma2);
                        cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXYZ(X,Y,Z,c));
                        S+=coef;
                      }
                      X+=u; Y+=v; Z+=w;
                    }
                  } break;
                  }
                  Tfloat *ptrd = res.data(x,y,z);
                  if (S>0) cimg_forC(res,c) { *ptrd+=val[c]/S; ptrd+=whd; }
                  else cimg_forC(res,c) { *ptrd+=(Tfloat)((*this)(x,y,z,c)); ptrd+=whd; }
                }
              } _cimg_abort_catch_omp2
            }
          }
        } else { // 2d LIC algorithm
          for (float theta = cimg::mod(360.0f,da)/2.0f; theta<360; (theta+=da),++N) {
            const float thetar = (float)(theta*cimg::PI/180),
              vx = (float)(std::cos(thetar)), vy = (float)(std::sin(thetar));
            const t *pa = G.data(0,0,0,0), *pb = G.data(0,0,0,1), *pc = G.data(0,0,0,2);
            Tfloat *pd0 = W.data(0,0,0,0), *pd1 = W.data(0,0,0,1), *pd2 = W.data(0,0,0,2);
            cimg_forXY(G,xg,yg) {
              const t a = *(pa++), b = *(pb++), c = *(pc++);
              const float
                u = (float)(a*vx + b*vy),
                v = (float)(b*vx + c*vy),
                n = std::max(1e-5f,cimg::hypot(u,v)),
                dln = dl/n;
              *(pd0++) = (Tfloat)(u*dln);
              *(pd1++) = (Tfloat)(v*dln);
              *(pd2++) = (Tfloat)n;
            }

            cimg_abort_test;
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=256 && _height>=2) firstprivate(val))
            cimg_forY(*this,y) _cimg_abort_try_omp2 {
              cimg_abort_test2;
              cimg_forX(*this,x) {
                val.fill(0);
                const float
                  n = (float)W(x,y,0,2),
                  fsigma = (float)(n*sqrt2amplitude),
                  fsigma2 = 2*fsigma*fsigma,
                  length = gauss_prec*fsigma;
                float
                  S = 0,
                  X = (float)x,
                  Y = (float)y;
                switch (interpolation_type) {
                case 0 : { // Nearest-neighbor
                  for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1; l+=dl) {
                    const int
                      cx = (int)(X + 0.5f),
                      cy = (int)(Y + 0.5f);
                    const float
                      u = (float)W(cx,cy,0,0),
                      v = (float)W(cx,cy,0,1);
                    if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)(*this)(cx,cy,0,c); ++S; }
                    else {
                      const float coef = (float)std::exp(-l*l/fsigma2);
                      cimg_forC(*this,c) val[c]+=(Tfloat)(coef*(*this)(cx,cy,0,c));
                      S+=coef;
                    }
                    X+=u; Y+=v;
                  }
                } break;
                case 1 : { // Linear interpolation
                  for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1; l+=dl) {
                    const float
                      u = (float)(W._linear_atXY(X,Y,0,0)),
                      v = (float)(W._linear_atXY(X,Y,0,1));
                    if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXY(X,Y,0,c); ++S; }
                    else {
                      const float coef = (float)std::exp(-l*l/fsigma2);
                      cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXY(X,Y,0,c));
                      S+=coef;
                    }
                    X+=u; Y+=v;
                  }
                } break;
                default : { // 2nd-order Runge-kutta interpolation
                  for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1; l+=dl) {
                    const float
                      u0 = (float)(0.5f*W._linear_atXY(X,Y,0,0)),
                      v0 = (float)(0.5f*W._linear_atXY(X,Y,0,1)),
                      u = (float)(W._linear_atXY(X + u0,Y + v0,0,0)),
                      v = (float)(W._linear_atXY(X + u0,Y + v0,0,1));
                    if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXY(X,Y,0,c); ++S; }
                    else {
                      const float coef = (float)std::exp(-l*l/fsigma2);
                      cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXY(X,Y,0,c));
                      S+=coef;
                    }
                    X+=u; Y+=v;
                  }
                }
                }
                Tfloat *ptrd = res.data(x,y);
                if (S>0) cimg_forC(res,c) { *ptrd+=val[c]/S; ptrd+=whd; }
                else cimg_forC(res,c) { *ptrd+=(Tfloat)((*this)(x,y,0,c)); ptrd+=whd; }
              }
            } _cimg_abort_catch_omp2
          }
        }
        const Tfloat *ptrs = res._data;
        cimg_for(*this,ptrd,T) {
          const Tfloat val = *(ptrs++)/N;
          *ptrd = val<val_min?val_min:(val>val_max?val_max:(T)val);
        }
      }
      cimg_abort_test;
      return *this;