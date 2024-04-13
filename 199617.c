    //! Compute Haar multiscale wavelet transform \newinstance.
    CImg<Tfloat> get_haar(const char axis, const bool invert=false, const unsigned int nb_scales=1) const {
      if (is_empty() || !nb_scales) return +*this;
      CImg<Tfloat> res;
      const Tfloat sqrt2 = std::sqrt(2.0f);
      if (nb_scales==1) {
        switch (cimg::lowercase(axis)) { // Single scale transform
        case 'x' : {
          const unsigned int w = _width/2;
          if (w) {
            if ((w%2) && w!=1)
              throw CImgInstanceException(_cimg_instance
                                          "haar(): Sub-image width %u is not even.",
                                          cimg_instance,
                                          w);

            res.assign(_width,_height,_depth,_spectrum);
            if (invert) cimg_forYZC(*this,y,z,c) { // Inverse transform along X
              for (unsigned int x = 0, xw = w, x2 = 0; x<w; ++x, ++xw) {
                const Tfloat val0 = (Tfloat)(*this)(x,y,z,c), val1 = (Tfloat)(*this)(xw,y,z,c);
                res(x2++,y,z,c) = (val0 - val1)/sqrt2;
                res(x2++,y,z,c) = (val0 + val1)/sqrt2;
              }
            } else cimg_forYZC(*this,y,z,c) { // Direct transform along X
              for (unsigned int x = 0, xw = w, x2 = 0; x<w; ++x, ++xw) {
                const Tfloat val0 = (Tfloat)(*this)(x2++,y,z,c), val1 = (Tfloat)(*this)(x2++,y,z,c);
                res(x,y,z,c) = (val0 + val1)/sqrt2;
                res(xw,y,z,c) = (val1 - val0)/sqrt2;
              }
            }
          } else return *this;
        } break;
        case 'y' : {
          const unsigned int h = _height/2;
          if (h) {
            if ((h%2) && h!=1)
              throw CImgInstanceException(_cimg_instance
                                          "haar(): Sub-image height %u is not even.",
                                          cimg_instance,
                                          h);

            res.assign(_width,_height,_depth,_spectrum);
            if (invert) cimg_forXZC(*this,x,z,c) { // Inverse transform along Y
              for (unsigned int y = 0, yh = h, y2 = 0; y<h; ++y, ++yh) {
                const Tfloat val0 = (Tfloat)(*this)(x,y,z,c), val1 = (Tfloat)(*this)(x,yh,z,c);
                res(x,y2++,z,c) = (val0 - val1)/sqrt2;
                res(x,y2++,z,c) = (val0 + val1)/sqrt2;
              }
            } else cimg_forXZC(*this,x,z,c) {
              for (unsigned int y = 0, yh = h, y2 = 0; y<h; ++y, ++yh) { // Direct transform along Y
                const Tfloat val0 = (Tfloat)(*this)(x,y2++,z,c), val1 = (Tfloat)(*this)(x,y2++,z,c);
                res(x,y,z,c)  = (val0 + val1)/sqrt2;
                res(x,yh,z,c) = (val1 - val0)/sqrt2;
              }
            }
          } else return *this;
        } break;
        case 'z' : {
          const unsigned int d = _depth/2;
          if (d) {
            if ((d%2) && d!=1)
              throw CImgInstanceException(_cimg_instance
                                          "haar(): Sub-image depth %u is not even.",
                                          cimg_instance,
                                          d);

            res.assign(_width,_height,_depth,_spectrum);
            if (invert) cimg_forXYC(*this,x,y,c) { // Inverse transform along Z
              for (unsigned int z = 0, zd = d, z2 = 0; z<d; ++z, ++zd) {
                const Tfloat val0 = (Tfloat)(*this)(x,y,z,c), val1 = (Tfloat)(*this)(x,y,zd,c);
                res(x,y,z2++,c) = (val0 - val1)/sqrt2;
                res(x,y,z2++,c) = (val0 + val1)/sqrt2;
              }
            } else cimg_forXYC(*this,x,y,c) {
              for (unsigned int z = 0, zd = d, z2 = 0; z<d; ++z, ++zd) { // Direct transform along Z
                const Tfloat val0 = (Tfloat)(*this)(x,y,z2++,c), val1 = (Tfloat)(*this)(x,y,z2++,c);
                res(x,y,z,c)  = (val0 + val1)/sqrt2;
                res(x,y,zd,c) = (val1 - val0)/sqrt2;
              }
            }
          } else return *this;
        } break;
        default :
          throw CImgArgumentException(_cimg_instance
                                      "haar(): Invalid specified axis '%c' "
                                      "(should be { x | y | z }).",
                                      cimg_instance,
                                      axis);
        }
      } else { // Multi-scale version
        if (invert) {
          res.assign(*this,false);
          switch (cimg::lowercase(axis)) {
          case 'x' : {
            unsigned int w = _width;
            for (unsigned int s = 1; w && s<nb_scales; ++s) w/=2;
            for (w = w?w:1; w<=_width; w*=2) res.draw_image(res.get_crop(0,w - 1).get_haar('x',true,1));
          } break;
          case 'y' : {
            unsigned int h = _width;
            for (unsigned int s = 1; h && s<nb_scales; ++s) h/=2;
            for (h = h?h:1; h<=_height; h*=2) res.draw_image(res.get_crop(0,0,_width - 1,h - 1).get_haar('y',true,1));
          } break;
          case 'z' : {
            unsigned int d = _depth;
            for (unsigned int s = 1; d && s<nb_scales; ++s) d/=2;
            for (d = d?d:1; d<=_depth; d*=2)
              res.draw_image(res.get_crop(0,0,0,_width - 1,_height - 1,d - 1).get_haar('z',true,1));
          } break;
          default :
            throw CImgArgumentException(_cimg_instance
                                        "haar(): Invalid specified axis '%c' "
                                        "(should be { x | y | z }).",
                                        cimg_instance,
                                        axis);
          }
        } else { // Direct transform
          res = get_haar(axis,false,1);
          switch (cimg::lowercase(axis)) {
          case 'x' : {
            for (unsigned int s = 1, w = _width/2; w && s<nb_scales; ++s, w/=2)
              res.draw_image(res.get_crop(0,w - 1).get_haar('x',false,1));
          } break;
          case 'y' : {
            for (unsigned int s = 1, h = _height/2; h && s<nb_scales; ++s, h/=2)
              res.draw_image(res.get_crop(0,0,_width - 1,h - 1).get_haar('y',false,1));
          } break;
          case 'z' : {
            for (unsigned int s = 1, d = _depth/2; d && s<nb_scales; ++s, d/=2)
              res.draw_image(res.get_crop(0,0,0,_width - 1,_height - 1,d - 1).get_haar('z',false,1));
          } break;
          default :
            throw CImgArgumentException(_cimg_instance
                                        "haar(): Invalid specified axis '%c' "
                                        "(should be { x | y | z }).",
                                        cimg_instance,
                                        axis);
          }
        }
      }
      return res;