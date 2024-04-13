    **/
    CImgList<Tfloat> get_gradient(const char *const axes=0, const int scheme=3) const {
      CImgList<Tfloat> grad(2,_width,_height,_depth,_spectrum);
      bool is_3d = false;
      if (axes) {
        for (unsigned int a = 0; axes[a]; ++a) {
          const char axis = cimg::lowercase(axes[a]);
          switch (axis) {
          case 'x' : case 'y' : break;
          case 'z' : is_3d = true; break;
          default :
            throw CImgArgumentException(_cimg_instance
                                        "get_gradient(): Invalid specified axis '%c'.",
                                        cimg_instance,
                                        axis);
          }
        }
      } else is_3d = (_depth>1);
      if (is_3d) {
        CImg<Tfloat>(_width,_height,_depth,_spectrum).move_to(grad);
        switch (scheme) { // 3d.
        case -1 : { // Backward finite differences.
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
          cimg_forC(*this,c) {
            const ulongT off = (ulongT)c*_width*_height*_depth;
            Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off, *ptrd2 = grad[2]._data + off;
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              *(ptrd0++) = Iccc - Ipcc;
              *(ptrd1++) = Iccc - Icpc;
              *(ptrd2++) = Iccc - Iccp;
            }
          }
        } break;
        case 1 : { // Forward finite differences.
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
          cimg_forC(*this,c) {
            const ulongT off = (ulongT)c*_width*_height*_depth;
            Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off, *ptrd2 = grad[2]._data + off;
            CImg_2x2x2(I,Tfloat);
            cimg_for2x2x2(*this,x,y,z,c,I,Tfloat) {
              *(ptrd0++) = Incc - Iccc;
              *(ptrd1++) = Icnc - Iccc;
              *(ptrd2++) = Iccn - Iccc;
            }
          }
        } break;
        case 4 : { // Deriche filter with low standard variation.
          grad[0] = get_deriche(0,1,'x');
          grad[1] = get_deriche(0,1,'y');
          grad[2] = get_deriche(0,1,'z');
        } break;
        case 5 : { // Van Vliet filter with low standard variation.
          grad[0] = get_vanvliet(0,1,'x');
          grad[1] = get_vanvliet(0,1,'y');
          grad[2] = get_vanvliet(0,1,'z');
        } break;
        default : { // Central finite differences.
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=1048576 && _spectrum>=2))
          cimg_forC(*this,c) {
            const ulongT off = (ulongT)c*_width*_height*_depth;
            Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off, *ptrd2 = grad[2]._data + off;
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              *(ptrd0++) = (Incc - Ipcc)/2;
              *(ptrd1++) = (Icnc - Icpc)/2;
              *(ptrd2++) = (Iccn - Iccp)/2;
            }
          }
        }
        }
      } else switch (scheme) { // 2d.
      case -1 : { // Backward finite differences.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
        cimg_forZC(*this,z,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth + z*_width*_height;
          Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off;
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = Icc - Ipc;
            *(ptrd1++) = Icc - Icp;
          }
        }
      } break;
      case 1 : { // Forward finite differences.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
        cimg_forZC(*this,z,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth + z*_width*_height;
          Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off;
          CImg_2x2(I,Tfloat);
          cimg_for2x2(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = Inc - Icc;
            *(ptrd1++) = Icn - Icc;
          }
        }
      } break;
      case 2 : { // Sobel scheme.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
        cimg_forZC(*this,z,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth + z*_width*_height;
          Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off;
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = -Ipp - 2*Ipc - Ipn + Inp + 2*Inc + Inn;
            *(ptrd1++) = -Ipp - 2*Icp - Inp + Ipn + 2*Icn + Inn;
          }
        }
      } break;
      case 3 : { // Rotation invariant kernel.
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
        cimg_forZC(*this,z,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth + z*_width*_height;
          Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off;
          CImg_3x3(I,Tfloat);
          const Tfloat a = (Tfloat)(0.25f*(2 - std::sqrt(2.0f))), b = (Tfloat)(0.5f*(std::sqrt(2.0f) - 1));
          cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = -a*Ipp - b*Ipc - a*Ipn + a*Inp + b*Inc + a*Inn;
            *(ptrd1++) = -a*Ipp - b*Icp - a*Inp + a*Ipn + b*Icn + a*Inn;
          }
        }
      } break;
      case 4 : { // Van Vliet filter with low standard variation
        grad[0] = get_deriche(0,1,'x');
        grad[1] = get_deriche(0,1,'y');
      } break;
      case 5 : { // Deriche filter with low standard variation
        grad[0] = get_vanvliet(0,1,'x');
        grad[1] = get_vanvliet(0,1,'y');
      } break;
      default : { // Central finite differences
        cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width*_height>=1048576 && _depth*_spectrum>=2))
        cimg_forZC(*this,z,c) {
          const ulongT off = (ulongT)c*_width*_height*_depth + z*_width*_height;
          Tfloat *ptrd0 = grad[0]._data + off, *ptrd1 = grad[1]._data + off;
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
            *(ptrd0++) = (Inc - Ipc)/2;
            *(ptrd1++) = (Icn - Icp)/2;
          }
        }
      }
      }
      if (!axes) return grad;
      CImgList<Tfloat> res;
      for (unsigned int l = 0; axes[l]; ++l) {
        const char axis = cimg::lowercase(axes[l]);
        switch (axis) {
        case 'x' : res.insert(grad[0]); break;
        case 'y' : res.insert(grad[1]); break;
        case 'z' : res.insert(grad[2]); break;
        }
      }
      grad.assign();
      return res;