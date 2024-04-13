    //! Rotate image with arbitrary angle \newinstance.
    CImg<T> get_rotate(const float angle, const unsigned int interpolation=1,
                       const unsigned int boundary_conditions=0) const {
      if (is_empty()) return *this;
      CImg<T> res;
      const float nangle = cimg::mod(angle,360.0f);
      if (boundary_conditions!=1 && cimg::mod(nangle,90.0f)==0) { // Optimized version for orthogonal angles.
        const int wm1 = width() - 1, hm1 = height() - 1;
        const int iangle = (int)nangle/90;
        switch (iangle) {
        case 1 : { // 90 deg
          res.assign(_height,_width,_depth,_spectrum);
          T *ptrd = res._data;
          cimg_forXYZC(res,x,y,z,c) *(ptrd++) = (*this)(y,hm1 - x,z,c);
        } break;
        case 2 : { // 180 deg
          res.assign(_width,_height,_depth,_spectrum);
          T *ptrd = res._data;
          cimg_forXYZC(res,x,y,z,c) *(ptrd++) = (*this)(wm1 - x,hm1 - y,z,c);
        } break;
        case 3 : { // 270 deg
          res.assign(_height,_width,_depth,_spectrum);
          T *ptrd = res._data;
          cimg_forXYZC(res,x,y,z,c) *(ptrd++) = (*this)(wm1 - y,x,z,c);
        } break;
        default : // 0 deg
          return *this;
        }
      } else { // Generic angle
        const float
          rad = (float)(nangle*cimg::PI/180.0),
          ca = (float)std::cos(rad), sa = (float)std::sin(rad),
          ux = cimg::abs((_width - 1)*ca), uy = cimg::abs((_width - 1)*sa),
          vx = cimg::abs((_height - 1)*sa), vy = cimg::abs((_height - 1)*ca),
          w2 = 0.5f*(_width - 1), h2 = 0.5f*(_height - 1);
        res.assign((int)cimg::round(1 + ux + vx),(int)cimg::round(1 + uy + vy),_depth,_spectrum);
        const float rw2 = 0.5f*(res._width - 1), rh2 = 0.5f*(res._height - 1);
        _rotate(res,nangle,interpolation,boundary_conditions,w2,h2,rw2,rh2);
      }
      return res;