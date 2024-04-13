    //! Compute the structure tensor field of an image \newinstance.
    CImg<Tfloat> get_structure_tensors(const bool is_fwbw_scheme=false) const {
      if (is_empty()) return *this;
      CImg<Tfloat> res;
      if (_depth>1) { // 3d
        res.assign(_width,_height,_depth,6,0);
        if (!is_fwbw_scheme) { // Classical central finite differences
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat
              *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2),
              *ptrd3 = res.data(0,0,0,3), *ptrd4 = res.data(0,0,0,4), *ptrd5 = res.data(0,0,0,5);
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat
                ix = (Incc - Ipcc)/2,
                iy = (Icnc - Icpc)/2,
                iz = (Iccn - Iccp)/2;
              *(ptrd0++)+=ix*ix;
              *(ptrd1++)+=ix*iy;
              *(ptrd2++)+=ix*iz;
              *(ptrd3++)+=iy*iy;
              *(ptrd4++)+=iy*iz;
              *(ptrd5++)+=iz*iz;
            }
          }
        } else { // Forward/backward finite differences.
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat
              *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2),
              *ptrd3 = res.data(0,0,0,3), *ptrd4 = res.data(0,0,0,4), *ptrd5 = res.data(0,0,0,5);
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat
                ixf = Incc - Iccc, ixb = Iccc - Ipcc,
                iyf = Icnc - Iccc, iyb = Iccc - Icpc,
                izf = Iccn - Iccc, izb = Iccc - Iccp;
              *(ptrd0++)+=(ixf*ixf + ixb*ixb)/2;
              *(ptrd1++)+=(ixf*iyf + ixf*iyb + ixb*iyf + ixb*iyb)/4;
              *(ptrd2++)+=(ixf*izf + ixf*izb + ixb*izf + ixb*izb)/4;
              *(ptrd3++)+=(iyf*iyf + iyb*iyb)/2;
              *(ptrd4++)+=(iyf*izf + iyf*izb + iyb*izf + iyb*izb)/4;
              *(ptrd5++)+=(izf*izf + izb*izb)/2;
            }
          }
        }
      } else { // 2d
        res.assign(_width,_height,_depth,3,0);
        if (!is_fwbw_scheme) { // Classical central finite differences
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2);
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat
                ix = (Inc - Ipc)/2,
                iy = (Icn - Icp)/2;
              *(ptrd0++)+=ix*ix;
              *(ptrd1++)+=ix*iy;
              *(ptrd2++)+=iy*iy;
            }
          }
        } else { // Forward/backward finite differences (version 2).
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2);
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat
                ixf = Inc - Icc, ixb = Icc - Ipc,
                iyf = Icn - Icc, iyb = Icc - Icp;
              *(ptrd0++)+=(ixf*ixf + ixb*ixb)/2;
              *(ptrd1++)+=(ixf*iyf + ixf*iyb + ixb*iyf + ixb*iyb)/4;
              *(ptrd2++)+=(iyf*iyf + iyb*iyb)/2;
            }
          }
        }
      }
      return res;