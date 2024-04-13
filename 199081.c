    template<typename t>
    CImg<T>& blur_bilateral(const CImg<t>& guide,
                            const float sigma_x, const float sigma_y,
                            const float sigma_z, const float sigma_r,
                            const float sampling_x, const float sampling_y,
                            const float sampling_z, const float sampling_r) {
      if (!is_sameXYZ(guide))
        throw CImgArgumentException(_cimg_instance
                                    "blur_bilateral(): Invalid size for specified guide image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      if (is_empty() || (!sigma_x && !sigma_y && !sigma_z)) return *this;
      T edge_min, edge_max = guide.max_min(edge_min);
      if (edge_min==edge_max) return blur(sigma_x,sigma_y,sigma_z);
      const float
        edge_delta = (float)(edge_max - edge_min),
        _sigma_x = sigma_x>=0?sigma_x:-sigma_x*_width/100,
        _sigma_y = sigma_y>=0?sigma_y:-sigma_y*_height/100,
        _sigma_z = sigma_z>=0?sigma_z:-sigma_z*_depth/100,
        _sigma_r = sigma_r>=0?sigma_r:-sigma_r*(edge_max - edge_min)/100,
        _sampling_x = sampling_x?sampling_x:std::max(_sigma_x,1.0f),
        _sampling_y = sampling_y?sampling_y:std::max(_sigma_y,1.0f),
        _sampling_z = sampling_z?sampling_z:std::max(_sigma_z,1.0f),
        _sampling_r = sampling_r?sampling_r:std::max(_sigma_r,edge_delta/256),
        derived_sigma_x = _sigma_x / _sampling_x,
        derived_sigma_y = _sigma_y / _sampling_y,
        derived_sigma_z = _sigma_z / _sampling_z,
        derived_sigma_r = _sigma_r / _sampling_r;
      const int
        padding_x = (int)(2*derived_sigma_x) + 1,
        padding_y = (int)(2*derived_sigma_y) + 1,
        padding_z = (int)(2*derived_sigma_z) + 1,
        padding_r = (int)(2*derived_sigma_r) + 1;
      const unsigned int
        bx = (unsigned int)((_width  - 1)/_sampling_x + 1 + 2*padding_x),
        by = (unsigned int)((_height - 1)/_sampling_y + 1 + 2*padding_y),
        bz = (unsigned int)((_depth  - 1)/_sampling_z + 1 + 2*padding_z),
        br = (unsigned int)(edge_delta/_sampling_r + 1 + 2*padding_r);
      if (bx>0 || by>0 || bz>0 || br>0) {
        const bool is_3d = (_depth>1);
        if (is_3d) { // 3d version of the algorithm
          CImg<floatT> bgrid(bx,by,bz,br), bgridw(bx,by,bz,br);
          cimg_forC(*this,c) {
            const CImg<t> _guide = guide.get_shared_channel(c%guide._spectrum);
            bgrid.fill(0); bgridw.fill(0);
            cimg_forXYZ(*this,x,y,z) {
              const T val = (*this)(x,y,z,c);
              const float edge = (float)_guide(x,y,z);
              const int
                X = (int)cimg::round(x/_sampling_x) + padding_x,
                Y = (int)cimg::round(y/_sampling_y) + padding_y,
                Z = (int)cimg::round(z/_sampling_z) + padding_z,
                R = (int)cimg::round((edge - edge_min)/_sampling_r) + padding_r;
              bgrid(X,Y,Z,R)+=(float)val;
              bgridw(X,Y,Z,R)+=1;
            }
            bgrid.blur(derived_sigma_x,derived_sigma_y,derived_sigma_z,true).deriche(derived_sigma_r,0,'c',false);
            bgridw.blur(derived_sigma_x,derived_sigma_y,derived_sigma_z,true).deriche(derived_sigma_r,0,'c',false);

            cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(size()>=4096))
            cimg_forXYZ(*this,x,y,z) {
              const float edge = (float)_guide(x,y,z);
              const float
                X = x/_sampling_x + padding_x,
                Y = y/_sampling_y + padding_y,
                Z = z/_sampling_z + padding_z,
                R = (edge - edge_min)/_sampling_r + padding_r;
              const float bval0 = bgrid._linear_atXYZC(X,Y,Z,R), bval1 = bgridw._linear_atXYZC(X,Y,Z,R);
              (*this)(x,y,z,c) = (T)(bval0/bval1);
            }
          }
        } else { // 2d version of the algorithm
          CImg<floatT> bgrid(bx,by,br,2);
          cimg_forC(*this,c) {
            const CImg<t> _guide = guide.get_shared_channel(c%guide._spectrum);
            bgrid.fill(0);
            cimg_forXY(*this,x,y) {
              const T val = (*this)(x,y,c);
              const float edge = (float)_guide(x,y);
              const int
                X = (int)cimg::round(x/_sampling_x) + padding_x,
                Y = (int)cimg::round(y/_sampling_y) + padding_y,
                R = (int)cimg::round((edge - edge_min)/_sampling_r) + padding_r;
              bgrid(X,Y,R,0)+=(float)val;
              bgrid(X,Y,R,1)+=1;
            }
            bgrid.blur(derived_sigma_x,derived_sigma_y,0,true).blur(0,0,derived_sigma_r,false);

            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(size()>=4096))
            cimg_forXY(*this,x,y) {
              const float edge = (float)_guide(x,y);
              const float
                X = x/_sampling_x + padding_x,
                Y = y/_sampling_y + padding_y,
                R = (edge - edge_min)/_sampling_r + padding_r;
              const float bval0 = bgrid._linear_atXYZ(X,Y,R,0), bval1 = bgrid._linear_atXYZ(X,Y,R,1);
              (*this)(x,y,c) = (T)(bval0/bval1);
            }
          }
        }
      }
      return *this;