    **/
    CImgList<Tfloat> get_hessian(const char *const axes=0) const {
      CImgList<Tfloat> res;
      const char *naxes = axes, *const def_axes2d = "xxxyyy", *const def_axes3d = "xxxyxzyyyzzz";
      if (!axes) naxes = _depth>1?def_axes3d:def_axes2d;
      const unsigned int lmax = (unsigned int)std::strlen(naxes);
      if (lmax%2)
        throw CImgArgumentException(_cimg_instance
                                    "get_hessian(): Invalid specified axes '%s'.",
                                    cimg_instance,
                                    naxes);

      res.assign(lmax/2,_width,_height,_depth,_spectrum);
      if (!cimg::strcasecmp(naxes,def_axes3d)) { // 3d

        cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
        cimg_forC(*this,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth;
          Tfloat
            *ptrd0 = res[0]._data + off, *ptrd1 = res[1]._data + off, *ptrd2 = res[2]._data + off,
            *ptrd3 = res[3]._data + off, *ptrd4 = res[4]._data + off, *ptrd5 = res[5]._data + off;
          CImg_3x3x3(I,Tfloat);
          cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = Ipcc + Incc - 2*Iccc;          // Ixx
            *(ptrd1++) = (Ippc + Innc - Ipnc - Inpc)/4; // Ixy
            *(ptrd2++) = (Ipcp + Incn - Ipcn - Incp)/4; // Ixz
            *(ptrd3++) = Icpc + Icnc - 2*Iccc;          // Iyy
            *(ptrd4++) = (Icpp + Icnn - Icpn - Icnp)/4; // Iyz
            *(ptrd5++) = Iccn + Iccp - 2*Iccc;          // Izz
          }
        }
      } else if (!cimg::strcasecmp(naxes,def_axes2d)) { // 2d
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
        cimg_forZC(*this,z,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth + z*_width*_height;
          Tfloat *ptrd0 = res[0]._data + off, *ptrd1 = res[1]._data + off, *ptrd2 = res[2]._data + off;
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = Ipc + Inc - 2*Icc;         // Ixx
            *(ptrd1++) = (Ipp + Inn - Ipn - Inp)/4; // Ixy
            *(ptrd2++) = Icp + Icn - 2*Icc;         // Iyy
          }
        }
      } else for (unsigned int l = 0; l<lmax; ) { // Version with custom axes.
          const unsigned int l2 = l/2;
          char axis1 = naxes[l++], axis2 = naxes[l++];
          if (axis1>axis2) cimg::swap(axis1,axis2);
          bool valid_axis = false;
          if (axis1=='x' && axis2=='x') { // Ixx
            valid_axis = true;
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              Tfloat *ptrd = res[l2].data(0,0,z,c);
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = Ipc + Inc - 2*Icc;
            }
          }
          else if (axis1=='x' && axis2=='y') { // Ixy
            valid_axis = true;
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              Tfloat *ptrd = res[l2].data(0,0,z,c);
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = (Ipp + Inn - Ipn - Inp)/4;
            }
          }
          else if (axis1=='x' && axis2=='z') { // Ixz
            valid_axis = true;
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
            cimg_forC(*this,c) {
              Tfloat *ptrd = res[l2].data(0,0,0,c);
              CImg_3x3x3(I,Tfloat);
              cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = (Ipcp + Incn - Ipcn - Incp)/4;
            }
          }
          else if (axis1=='y' && axis2=='y') { // Iyy
            valid_axis = true;
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              Tfloat *ptrd = res[l2].data(0,0,z,c);
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = Icp + Icn - 2*Icc;
            }
          }
          else if (axis1=='y' && axis2=='z') { // Iyz
            valid_axis = true;
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
            cimg_forC(*this,c) {
              Tfloat *ptrd = res[l2].data(0,0,0,c);
              CImg_3x3x3(I,Tfloat);
              cimg_forC(*this,c) cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = (Icpp + Icnn - Icpn - Icnp)/4;
            }
          }
          else if (axis1=='z' && axis2=='z') { // Izz
            valid_axis = true;
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
            cimg_forC(*this,c) {
              Tfloat *ptrd = res[l2].data(0,0,0,c);
              CImg_3x3x3(I,Tfloat);
              cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = Iccn + Iccp - 2*Iccc;
            }
          }
          else if (!valid_axis)
            throw CImgArgumentException(_cimg_instance
                                        "get_hessian(): Invalid specified axes '%s'.",
                                        cimg_instance,
                                        naxes);
        }
      return res;