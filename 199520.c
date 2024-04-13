    //! Resize image to new dimensions \newinstance.
    CImg<T> get_resize(const int size_x, const int size_y = -100,
                       const int size_z = -100, const int size_c = -100,
                       const int interpolation_type=1, const unsigned int boundary_conditions=0,
                       const float centering_x = 0, const float centering_y = 0,
                       const float centering_z = 0, const float centering_c = 0) const {
      if (centering_x<0 || centering_x>1 || centering_y<0 || centering_y>1 ||
          centering_z<0 || centering_z>1 || centering_c<0 || centering_c>1)
        throw CImgArgumentException(_cimg_instance
                                    "resize(): Specified centering arguments (%g,%g,%g,%g) are outside range [0,1].",
                                    cimg_instance,
                                    centering_x,centering_y,centering_z,centering_c);

      if (!size_x || !size_y || !size_z || !size_c) return CImg<T>();
      const unsigned int
        sx = std::max(1U,(unsigned int)(size_x>=0?size_x:-size_x*width()/100)),
        sy = std::max(1U,(unsigned int)(size_y>=0?size_y:-size_y*height()/100)),
        sz = std::max(1U,(unsigned int)(size_z>=0?size_z:-size_z*depth()/100)),
        sc = std::max(1U,(unsigned int)(size_c>=0?size_c:-size_c*spectrum()/100));
      if (sx==_width && sy==_height && sz==_depth && sc==_spectrum) return +*this;
      if (is_empty()) return CImg<T>(sx,sy,sz,sc,(T)0);
      CImg<T> res;
      switch (interpolation_type) {

        // Raw resizing.
        //
      case -1 :
        std::memcpy(res.assign(sx,sy,sz,sc,(T)0)._data,_data,sizeof(T)*std::min(size(),(ulongT)sx*sy*sz*sc));
        break;

        // No interpolation.
        //
      case 0 : {
        const int
          xc = (int)(centering_x*((int)sx - width())),
          yc = (int)(centering_y*((int)sy - height())),
          zc = (int)(centering_z*((int)sz - depth())),
          cc = (int)(centering_c*((int)sc - spectrum()));

        switch (boundary_conditions) {
        case 3 : { // Mirror
          res.assign(sx,sy,sz,sc);
          const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth(), s2 = 2*spectrum();
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=65536))
          cimg_forXYZC(res,x,y,z,c) {
            const int
              mx = cimg::mod(x - xc,w2), my = cimg::mod(y - yc,h2),
              mz = cimg::mod(z - zc,d2), mc = cimg::mod(c - cc,s2);
            res(x,y,z,c) = (*this)(mx<width()?mx:w2 - mx - 1,
                                   my<height()?my:h2 - my - 1,
                                   mz<depth()?mz:d2 - mz - 1,
                                   mc<spectrum()?mc:s2 - mc - 1);
          }
        } break;
        case 2 : { // Periodic
          res.assign(sx,sy,sz,sc);
          const int
            x0 = ((int)xc%width()) - width(),
            y0 = ((int)yc%height()) - height(),
            z0 = ((int)zc%depth()) - depth(),
            c0 = ((int)cc%spectrum()) - spectrum(),
            dx = width(), dy = height(), dz = depth(), dc = spectrum();
          cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(res.size()>=65536))
          for (int c = c0; c<(int)sc; c+=dc)
            for (int z = z0; z<(int)sz; z+=dz)
              for (int y = y0; y<(int)sy; y+=dy)
                for (int x = x0; x<(int)sx; x+=dx)
                  res.draw_image(x,y,z,c,*this);
        } break;
        case 1 : { // Neumann
          res.assign(sx,sy,sz,sc).draw_image(xc,yc,zc,cc,*this);
          CImg<T> sprite;
          if (xc>0) {  // X-backward
            res.get_crop(xc,yc,zc,cc,xc,yc + height() - 1,zc + depth() - 1,cc + spectrum() - 1).move_to(sprite);
            for (int x = xc - 1; x>=0; --x) res.draw_image(x,yc,zc,cc,sprite);
          }
          if (xc + width()<(int)sx) { // X-forward
            res.get_crop(xc + width() - 1,yc,zc,cc,xc + width() - 1,yc + height() - 1,
                         zc + depth() - 1,cc + spectrum() - 1).move_to(sprite);
            for (int x = xc + width(); x<(int)sx; ++x) res.draw_image(x,yc,zc,cc,sprite);
          }
          if (yc>0) {  // Y-backward
            res.get_crop(0,yc,zc,cc,sx - 1,yc,zc + depth() - 1,cc + spectrum() - 1).move_to(sprite);
            for (int y = yc - 1; y>=0; --y) res.draw_image(0,y,zc,cc,sprite);
          }
          if (yc + height()<(int)sy) { // Y-forward
            res.get_crop(0,yc + height() - 1,zc,cc,sx - 1,yc + height() - 1,
                         zc + depth() - 1,cc + spectrum() - 1).move_to(sprite);
            for (int y = yc + height(); y<(int)sy; ++y) res.draw_image(0,y,zc,cc,sprite);
          }
          if (zc>0) {  // Z-backward
            res.get_crop(0,0,zc,cc,sx - 1,sy - 1,zc,cc + spectrum() - 1).move_to(sprite);
            for (int z = zc - 1; z>=0; --z) res.draw_image(0,0,z,cc,sprite);
          }
          if (zc + depth()<(int)sz) { // Z-forward
            res.get_crop(0,0,zc  +depth() - 1,cc,sx - 1,sy - 1,zc + depth() - 1,cc + spectrum() - 1).move_to(sprite);
            for (int z = zc + depth(); z<(int)sz; ++z) res.draw_image(0,0,z,cc,sprite);
          }
          if (cc>0) {  // C-backward
            res.get_crop(0,0,0,cc,sx - 1,sy - 1,sz - 1,cc).move_to(sprite);
            for (int c = cc - 1; c>=0; --c) res.draw_image(0,0,0,c,sprite);
          }
          if (cc + spectrum()<(int)sc) { // C-forward
            res.get_crop(0,0,0,cc + spectrum() - 1,sx - 1,sy - 1,sz - 1,cc + spectrum() - 1).move_to(sprite);
            for (int c = cc + spectrum(); c<(int)sc; ++c) res.draw_image(0,0,0,c,sprite);
          }
        } break;
        default : // Dirichlet
          res.assign(sx,sy,sz,sc,(T)0).draw_image(xc,yc,zc,cc,*this);
        }
        break;
      } break;

        // Nearest neighbor interpolation.
        //
      case 1 : {
        res.assign(sx,sy,sz,sc);
        CImg<ulongT> off_x(sx), off_y(sy + 1), off_z(sz + 1), off_c(sc + 1);
        const ulongT
          wh = (ulongT)_width*_height,
          whd = (ulongT)_width*_height*_depth,
          sxy = (ulongT)sx*sy,
          sxyz = (ulongT)sx*sy*sz;
        if (sx==_width) off_x.fill(1);
        else {
          ulongT *poff_x = off_x._data, curr = 0;
          cimg_forX(res,x) {
            const ulongT old = curr;
            curr = (ulongT)((x + 1.0)*_width/sx);
            *(poff_x++) = curr - old;
          }
        }
        if (sy==_height) off_y.fill(_width);
        else {
          ulongT *poff_y = off_y._data, curr = 0;
          cimg_forY(res,y) {
            const ulongT old = curr;
            curr = (ulongT)((y + 1.0)*_height/sy);
            *(poff_y++) = _width*(curr - old);
          }
          *poff_y = 0;
        }
        if (sz==_depth) off_z.fill(wh);
        else {
          ulongT *poff_z = off_z._data, curr = 0;
          cimg_forZ(res,z) {
            const ulongT old = curr;
            curr = (ulongT)((z + 1.0)*_depth/sz);
            *(poff_z++) = wh*(curr - old);
          }
          *poff_z = 0;
        }
        if (sc==_spectrum) off_c.fill(whd);
        else {
          ulongT *poff_c = off_c._data, curr = 0;
          cimg_forC(res,c) {
            const ulongT old = curr;
            curr = (ulongT)((c + 1.0)*_spectrum/sc);
            *(poff_c++) = whd*(curr - old);
          }
          *poff_c = 0;
        }

        T *ptrd = res._data;
        const T* ptrc = _data;
        const ulongT *poff_c = off_c._data;
        for (unsigned int c = 0; c<sc; ) {
          const T *ptrz = ptrc;
          const ulongT *poff_z = off_z._data;
          for (unsigned int z = 0; z<sz; ) {
            const T *ptry = ptrz;
            const ulongT *poff_y = off_y._data;
            for (unsigned int y = 0; y<sy; ) {
              const T *ptrx = ptry;
              const ulongT *poff_x = off_x._data;
              cimg_forX(res,x) { *(ptrd++) = *ptrx; ptrx+=*(poff_x++); }
              ++y;
              ulongT dy = *(poff_y++);
              for ( ; !dy && y<dy; std::memcpy(ptrd,ptrd - sx,sizeof(T)*sx), ++y, ptrd+=sx, dy = *(poff_y++)) {}
              ptry+=dy;
            }
            ++z;
            ulongT dz = *(poff_z++);
            for ( ; !dz && z<dz; std::memcpy(ptrd,ptrd-sxy,sizeof(T)*sxy), ++z, ptrd+=sxy, dz = *(poff_z++)) {}
            ptrz+=dz;
          }
          ++c;
          ulongT dc = *(poff_c++);
          for ( ; !dc && c<dc; std::memcpy(ptrd,ptrd-sxyz,sizeof(T)*sxyz), ++c, ptrd+=sxyz, dc = *(poff_c++)) {}
          ptrc+=dc;
        }
      } break;

        // Moving average.
        //
      case 2 : {
        bool instance_first = true;
        if (sx!=_width) {
          CImg<Tfloat> tmp(sx,_height,_depth,_spectrum,0);
          for (unsigned int a = _width*sx, b = _width, c = sx, s = 0, t = 0; a; ) {
            const unsigned int d = std::min(b,c);
            a-=d; b-=d; c-=d;
            cimg_forYZC(tmp,y,z,v) tmp(t,y,z,v)+=(Tfloat)(*this)(s,y,z,v)*d;
            if (!b) {
              cimg_forYZC(tmp,y,z,v) tmp(t,y,z,v)/=_width;
              ++t;
              b = _width;
            }
            if (!c) { ++s; c = sx; }
          }
          tmp.move_to(res);
          instance_first = false;
        }
        if (sy!=_height) {
          CImg<Tfloat> tmp(sx,sy,_depth,_spectrum,0);
          for (unsigned int a = _height*sy, b = _height, c = sy, s = 0, t = 0; a; ) {
            const unsigned int d = std::min(b,c);
            a-=d; b-=d; c-=d;
            if (instance_first)
              cimg_forXZC(tmp,x,z,v) tmp(x,t,z,v)+=(Tfloat)(*this)(x,s,z,v)*d;
            else
              cimg_forXZC(tmp,x,z,v) tmp(x,t,z,v)+=(Tfloat)res(x,s,z,v)*d;
            if (!b) {
              cimg_forXZC(tmp,x,z,v) tmp(x,t,z,v)/=_height;
              ++t;
              b = _height;
            }
            if (!c) { ++s; c = sy; }
          }
          tmp.move_to(res);
          instance_first = false;
        }
        if (sz!=_depth) {
          CImg<Tfloat> tmp(sx,sy,sz,_spectrum,0);
          for (unsigned int a = _depth*sz, b = _depth, c = sz, s = 0, t = 0; a; ) {
            const unsigned int d = std::min(b,c);
            a-=d; b-=d; c-=d;
            if (instance_first)
              cimg_forXYC(tmp,x,y,v) tmp(x,y,t,v)+=(Tfloat)(*this)(x,y,s,v)*d;
            else
              cimg_forXYC(tmp,x,y,v) tmp(x,y,t,v)+=(Tfloat)res(x,y,s,v)*d;
            if (!b) {
              cimg_forXYC(tmp,x,y,v) tmp(x,y,t,v)/=_depth;
              ++t;
              b = _depth;
            }
            if (!c) { ++s; c = sz; }
          }
          tmp.move_to(res);
          instance_first = false;
        }
        if (sc!=_spectrum) {
          CImg<Tfloat> tmp(sx,sy,sz,sc,0);
          for (unsigned int a = _spectrum*sc, b = _spectrum, c = sc, s = 0, t = 0; a; ) {
            const unsigned int d = std::min(b,c);
            a-=d; b-=d; c-=d;
            if (instance_first)
              cimg_forXYZ(tmp,x,y,z) tmp(x,y,z,t)+=(Tfloat)(*this)(x,y,z,s)*d;
            else
              cimg_forXYZ(tmp,x,y,z) tmp(x,y,z,t)+=(Tfloat)res(x,y,z,s)*d;
            if (!b) {
              cimg_forXYZ(tmp,x,y,z) tmp(x,y,z,t)/=_spectrum;
              ++t;
              b = _spectrum;
            }
            if (!c) { ++s; c = sc; }
          }
          tmp.move_to(res);
          instance_first = false;
        }
      } break;

        // Linear interpolation.
        //
      case 3 : {
        CImg<uintT> off(cimg::max(sx,sy,sz,sc));
        CImg<doubleT> foff(off._width);
        CImg<T> resx, resy, resz, resc;
        double curr, old;

        if (sx!=_width) {
          if (_width==1) get_resize(sx,_height,_depth,_spectrum,1).move_to(resx);
          else if (_width>sx) get_resize(sx,_height,_depth,_spectrum,2).move_to(resx);
          else {
            const double fx = (!boundary_conditions && sx>_width)?(sx>1?(_width - 1.0)/(sx - 1):0):
              (double)_width/sx;
            resx.assign(sx,_height,_depth,_spectrum);
            curr = old = 0;
            unsigned int *poff = off._data;
            double *pfoff = foff._data;
            cimg_forX(resx,x) {
              *(pfoff++) = curr - (unsigned int)curr;
              old = curr;
              curr = std::min(width() - 1.0,curr + fx);
              *(poff++) = (unsigned int)curr - (unsigned int)old;
            }
            cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resx.size()>=65536))
              cimg_forYZC(resx,y,z,c) {
              const T *ptrs = data(0,y,z,c), *const ptrsmax = ptrs + _width - 1;
              T *ptrd = resx.data(0,y,z,c);
              const unsigned int *poff = off._data;
              const double *pfoff = foff._data;
              cimg_forX(resx,x) {
                const double alpha = *(pfoff++);
                const T val1 = *ptrs, val2 = ptrs<ptrsmax?*(ptrs + 1):val1;
                *(ptrd++) = (T)((1 - alpha)*val1 + alpha*val2);
                ptrs+=*(poff++);
              }
            }
          }
        } else resx.assign(*this,true);

        if (sy!=_height) {
          if (_height==1) resx.get_resize(sx,sy,_depth,_spectrum,1).move_to(resy);
          else {
            if (_height>sy) resx.get_resize(sx,sy,_depth,_spectrum,2).move_to(resy);
            else {
              const double fy = (!boundary_conditions && sy>_height)?(sy>1?(_height - 1.0)/(sy - 1):0):
                (double)_height/sy;
              resy.assign(sx,sy,_depth,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forY(resy,y) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(height() - 1.0,curr + fy);
                *(poff++) = sx*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resy.size()>=65536))
              cimg_forXZC(resy,x,z,c) {
                const T *ptrs = resx.data(x,0,z,c), *const ptrsmax = ptrs + (_height - 1)*sx;
                T *ptrd = resy.data(x,0,z,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forY(resy,y) {
                  const double alpha = *(pfoff++);
                  const T val1 = *ptrs, val2 = ptrs<ptrsmax?*(ptrs + sx):val1;
                  *ptrd = (T)((1 - alpha)*val1 + alpha*val2);
                  ptrd+=sx;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resx.assign();
        } else resy.assign(resx,true);

        if (sz!=_depth) {
          if (_depth==1) resy.get_resize(sx,sy,sz,_spectrum,1).move_to(resz);
          else {
            if (_depth>sz) resy.get_resize(sx,sy,sz,_spectrum,2).move_to(resz);
            else {
              const double fz = (!boundary_conditions && sz>_depth)?(sz>1?(_depth - 1.0)/(sz - 1):0):
                (double)_depth/sz;
              const unsigned int sxy = sx*sy;
              resz.assign(sx,sy,sz,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forZ(resz,z) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(depth() - 1.0,curr + fz);
                *(poff++) = sxy*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resz.size()>=65536))
              cimg_forXYC(resz,x,y,c) {
                const T *ptrs = resy.data(x,y,0,c), *const ptrsmax = ptrs + (_depth - 1)*sxy;
                T *ptrd = resz.data(x,y,0,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forZ(resz,z) {
                  const double alpha = *(pfoff++);
                  const T val1 = *ptrs, val2 = ptrs<ptrsmax?*(ptrs + sxy):val1;
                  *ptrd = (T)((1 - alpha)*val1 + alpha*val2);
                  ptrd+=sxy;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resy.assign();
        } else resz.assign(resy,true);

        if (sc!=_spectrum) {
          if (_spectrum==1) resz.get_resize(sx,sy,sz,sc,1).move_to(resc);
          else {
            if (_spectrum>sc) resz.get_resize(sx,sy,sz,sc,2).move_to(resc);
            else {
              const double fc = (!boundary_conditions && sc>_spectrum)?(sc>1?(_spectrum - 1.0)/(sc - 1):0):
                (double)_spectrum/sc;
              const unsigned int sxyz = sx*sy*sz;
              resc.assign(sx,sy,sz,sc);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forC(resc,c) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(spectrum() - 1.0,curr + fc);
                *(poff++) = sxyz*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resc.size()>=65536))
              cimg_forXYZ(resc,x,y,z) {
                const T *ptrs = resz.data(x,y,z,0), *const ptrsmax = ptrs + (_spectrum - 1)*sxyz;
                T *ptrd = resc.data(x,y,z,0);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forC(resc,c) {
                  const double alpha = *(pfoff++);
                  const T val1 = *ptrs, val2 = ptrs<ptrsmax?*(ptrs + sxyz):val1;
                  *ptrd = (T)((1 - alpha)*val1 + alpha*val2);
                  ptrd+=sxyz;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resz.assign();
        } else resc.assign(resz,true);
        return resc._is_shared?(resz._is_shared?(resy._is_shared?(resx._is_shared?(+(*this)):resx):resy):resz):resc;
      } break;

        // Grid interpolation.
        //
      case 4 : {
        CImg<T> resx, resy, resz, resc;
        if (sx!=_width) {
          if (sx<_width) get_resize(sx,_height,_depth,_spectrum,1).move_to(resx);
          else {
            resx.assign(sx,_height,_depth,_spectrum,(T)0);
            const int dx = (int)(2*sx), dy = 2*width();
            int err = (int)(dy + centering_x*(sx*dy/width() - dy)), xs = 0;
            cimg_forX(resx,x) if ((err-=dy)<=0) {
              cimg_forYZC(resx,y,z,c) resx(x,y,z,c) = (*this)(xs,y,z,c);
              ++xs;
              err+=dx;
            }
          }
        } else resx.assign(*this,true);

        if (sy!=_height) {
          if (sy<_height) resx.get_resize(sx,sy,_depth,_spectrum,1).move_to(resy);
          else {
            resy.assign(sx,sy,_depth,_spectrum,(T)0);
            const int dx = (int)(2*sy), dy = 2*height();
            int err = (int)(dy + centering_y*(sy*dy/height() - dy)), ys = 0;
            cimg_forY(resy,y) if ((err-=dy)<=0) {
              cimg_forXZC(resy,x,z,c) resy(x,y,z,c) = resx(x,ys,z,c);
              ++ys;
              err+=dx;
            }
          }
          resx.assign();
        } else resy.assign(resx,true);

        if (sz!=_depth) {
          if (sz<_depth) resy.get_resize(sx,sy,sz,_spectrum,1).move_to(resz);
          else {
            resz.assign(sx,sy,sz,_spectrum,(T)0);
            const int dx = (int)(2*sz), dy = 2*depth();
            int err = (int)(dy + centering_z*(sz*dy/depth() - dy)), zs = 0;
            cimg_forZ(resz,z) if ((err-=dy)<=0) {
              cimg_forXYC(resz,x,y,c) resz(x,y,z,c) = resy(x,y,zs,c);
              ++zs;
              err+=dx;
            }
          }
          resy.assign();
        } else resz.assign(resy,true);

        if (sc!=_spectrum) {
          if (sc<_spectrum) resz.get_resize(sx,sy,sz,sc,1).move_to(resc);
          else {
            resc.assign(sx,sy,sz,sc,(T)0);
            const int dx = (int)(2*sc), dy = 2*spectrum();
            int err = (int)(dy + centering_c*(sc*dy/spectrum() - dy)), cs = 0;
            cimg_forC(resc,c) if ((err-=dy)<=0) {
              cimg_forXYZ(resc,x,y,z) resc(x,y,z,c) = resz(x,y,z,cs);
              ++cs;
              err+=dx;
            }
          }
          resz.assign();
        } else resc.assign(resz,true);

        return resc._is_shared?(resz._is_shared?(resy._is_shared?(resx._is_shared?(+(*this)):resx):resy):resz):resc;
      } break;

        // Cubic interpolation.
        //
      case 5 : {
        const Tfloat vmin = (Tfloat)cimg::type<T>::min(), vmax = (Tfloat)cimg::type<T>::max();
        CImg<uintT> off(cimg::max(sx,sy,sz,sc));
        CImg<doubleT> foff(off._width);
        CImg<T> resx, resy, resz, resc;
        double curr, old;

        if (sx!=_width) {
          if (_width==1) get_resize(sx,_height,_depth,_spectrum,1).move_to(resx);
          else {
            if (_width>sx) get_resize(sx,_height,_depth,_spectrum,2).move_to(resx);
            else {
              const double fx = (!boundary_conditions && sx>_width)?(sx>1?(_width - 1.0)/(sx - 1):0):
                (double)_width/sx;
              resx.assign(sx,_height,_depth,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forX(resx,x) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(width() - 1.0,curr + fx);
                *(poff++) = (unsigned int)curr - (unsigned int)old;
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resx.size()>=65536))
              cimg_forYZC(resx,y,z,c) {
                const T *const ptrs0 = data(0,y,z,c), *ptrs = ptrs0, *const ptrsmax = ptrs + (_width - 2);
                T *ptrd = resx.data(0,y,z,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forX(resx,x) {
                  const double
                    t = *(pfoff++),
                    val1 = (double)*ptrs,
                    val0 = ptrs>ptrs0?(double)*(ptrs - 1):val1,
                    val2 = ptrs<=ptrsmax?(double)*(ptrs + 1):val1,
                    val3 = ptrs<ptrsmax?(double)*(ptrs + 2):val2,
                    val = val1 + 0.5f*(t*(-val0 + val2) + t*t*(2*val0 - 5*val1 + 4*val2 - val3) +
                                       t*t*t*(-val0 + 3*val1 - 3*val2 + val3));
                  *(ptrd++) = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrs+=*(poff++);
                }
              }
            }
          }
        } else resx.assign(*this,true);

        if (sy!=_height) {
          if (_height==1) resx.get_resize(sx,sy,_depth,_spectrum,1).move_to(resy);
          else {
            if (_height>sy) resx.get_resize(sx,sy,_depth,_spectrum,2).move_to(resy);
            else {
              const double fy = (!boundary_conditions && sy>_height)?(sy>1?(_height - 1.0)/(sy - 1):0):
                (double)_height/sy;
              resy.assign(sx,sy,_depth,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forY(resy,y) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(height() - 1.0,curr + fy);
                *(poff++) = sx*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resy.size()>=65536))
              cimg_forXZC(resy,x,z,c) {
                const T *const ptrs0 = resx.data(x,0,z,c), *ptrs = ptrs0, *const ptrsmax = ptrs + (_height - 2)*sx;
                T *ptrd = resy.data(x,0,z,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forY(resy,y) {
                  const double
                    t = *(pfoff++),
                    val1 = (double)*ptrs,
                    val0 = ptrs>ptrs0?(double)*(ptrs - sx):val1,
                    val2 = ptrs<=ptrsmax?(double)*(ptrs + sx):val1,
                    val3 = ptrs<ptrsmax?(double)*(ptrs + 2*sx):val2,
                    val = val1 + 0.5f*(t*(-val0 + val2) + t*t*(2*val0 - 5*val1 + 4*val2 - val3) +
                                       t*t*t*(-val0 + 3*val1 - 3*val2 + val3));
                  *ptrd = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrd+=sx;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resx.assign();
        } else resy.assign(resx,true);

        if (sz!=_depth) {
          if (_depth==1) resy.get_resize(sx,sy,sz,_spectrum,1).move_to(resz);
          else {
            if (_depth>sz) resy.get_resize(sx,sy,sz,_spectrum,2).move_to(resz);
            else {
              const double fz = (!boundary_conditions && sz>_depth)?(sz>1?(_depth - 1.0)/(sz - 1):0):
                (double)_depth/sz;
              const unsigned int sxy = sx*sy;
              resz.assign(sx,sy,sz,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forZ(resz,z) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(depth() - 1.0,curr + fz);
                *(poff++) = sxy*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resz.size()>=65536))
              cimg_forXYC(resz,x,y,c) {
                const T *const ptrs0 = resy.data(x,y,0,c), *ptrs = ptrs0, *const ptrsmax = ptrs + (_depth - 2)*sxy;
                T *ptrd = resz.data(x,y,0,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forZ(resz,z) {
                  const double
                    t = *(pfoff++),
                    val1 = (double)*ptrs,
                    val0 = ptrs>ptrs0?(double)*(ptrs - sxy):val1,
                    val2 = ptrs<=ptrsmax?(double)*(ptrs + sxy):val1,
                    val3 = ptrs<ptrsmax?(double)*(ptrs + 2*sxy):val2,
                    val = val1 + 0.5f*(t*(-val0 + val2) + t*t*(2*val0 - 5*val1 + 4*val2 - val3) +
                                       t*t*t*(-val0 + 3*val1 - 3*val2 + val3));
                  *ptrd = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrd+=sxy;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resy.assign();
        } else resz.assign(resy,true);

        if (sc!=_spectrum) {
          if (_spectrum==1) resz.get_resize(sx,sy,sz,sc,1).move_to(resc);
          else {
            if (_spectrum>sc) resz.get_resize(sx,sy,sz,sc,2).move_to(resc);
            else {
              const double fc = (!boundary_conditions && sc>_spectrum)?(sc>1?(_spectrum - 1.0)/(sc - 1):0):
                (double)_spectrum/sc;
              const unsigned int sxyz = sx*sy*sz;
              resc.assign(sx,sy,sz,sc);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forC(resc,c) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(spectrum() - 1.0,curr + fc);
                *(poff++) = sxyz*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resc.size()>=65536))
              cimg_forXYZ(resc,x,y,z) {
                const T *const ptrs0 = resz.data(x,y,z,0), *ptrs = ptrs0, *const ptrsmax = ptrs + (_spectrum - 2)*sxyz;
                T *ptrd = resc.data(x,y,z,0);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forC(resc,c) {
                  const double
                    t = *(pfoff++),
                    val1 = (double)*ptrs,
                    val0 = ptrs>ptrs0?(double)*(ptrs - sxyz):val1,
                    val2 = ptrs<=ptrsmax?(double)*(ptrs + sxyz):val1,
                    val3 = ptrs<ptrsmax?(double)*(ptrs + 2*sxyz):val2,
                    val = val1 + 0.5f*(t*(-val0 + val2) + t*t*(2*val0 - 5*val1 + 4*val2 - val3) +
                                       t*t*t*(-val0 + 3*val1 - 3*val2 + val3));
                  *ptrd = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrd+=sxyz;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resz.assign();
        } else resc.assign(resz,true);

        return resc._is_shared?(resz._is_shared?(resy._is_shared?(resx._is_shared?(+(*this)):resx):resy):resz):resc;
      } break;

        // Lanczos interpolation.
        //
      case 6 : {
        const double vmin = (double)cimg::type<T>::min(), vmax = (double)cimg::type<T>::max();
        CImg<uintT> off(cimg::max(sx,sy,sz,sc));
        CImg<doubleT> foff(off._width);
        CImg<T> resx, resy, resz, resc;
        double curr, old;

        if (sx!=_width) {
          if (_width==1) get_resize(sx,_height,_depth,_spectrum,1).move_to(resx);
          else {
            if (_width>sx) get_resize(sx,_height,_depth,_spectrum,2).move_to(resx);
            else {
              const double fx = (!boundary_conditions && sx>_width)?(sx>1?(_width - 1.0)/(sx - 1):0):
                (double)_width/sx;
              resx.assign(sx,_height,_depth,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forX(resx,x) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(width() - 1.0,curr + fx);
                *(poff++) = (unsigned int)curr - (unsigned int)old;
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resx.size()>=65536))
              cimg_forYZC(resx,y,z,c) {
                const T *const ptrs0 = data(0,y,z,c), *ptrs = ptrs0, *const ptrsmin = ptrs0 + 1,
                  *const ptrsmax = ptrs0 + (_width - 2);
                T *ptrd = resx.data(0,y,z,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forX(resx,x) {
                  const double
                    t = *(pfoff++),
                    w0 = _cimg_lanczos(t + 2),
                    w1 = _cimg_lanczos(t + 1),
                    w2 = _cimg_lanczos(t),
                    w3 = _cimg_lanczos(t - 1),
                    w4 = _cimg_lanczos(t - 2),
                    val2 = (double)*ptrs,
                    val1 = ptrs>=ptrsmin?(double)*(ptrs - 1):val2,
                    val0 = ptrs>ptrsmin?(double)*(ptrs - 2):val1,
                    val3 = ptrs<=ptrsmax?(double)*(ptrs + 1):val2,
                    val4 = ptrs<ptrsmax?(double)*(ptrs + 2):val3,
                    val = (val0*w0 + val1*w1 + val2*w2 + val3*w3 + val4*w4)/(w1 + w2 + w3 + w4);
                  *(ptrd++) = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrs+=*(poff++);
                }
              }
            }
          }
        } else resx.assign(*this,true);

        if (sy!=_height) {
          if (_height==1) resx.get_resize(sx,sy,_depth,_spectrum,1).move_to(resy);
          else {
            if (_height>sy) resx.get_resize(sx,sy,_depth,_spectrum,2).move_to(resy);
            else {
              const double fy = (!boundary_conditions && sy>_height)?(sy>1?(_height - 1.0)/(sy - 1):0):
                (double)_height/sy;
              resy.assign(sx,sy,_depth,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forY(resy,y) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(height() - 1.0,curr + fy);
                *(poff++) = sx*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resy.size()>=65536))
              cimg_forXZC(resy,x,z,c) {
                const T *const ptrs0 = resx.data(x,0,z,c), *ptrs = ptrs0, *const ptrsmin = ptrs0 + sx,
                  *const ptrsmax = ptrs0 + (_height - 2)*sx;
                T *ptrd = resy.data(x,0,z,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forY(resy,y) {
                  const double
                    t = *(pfoff++),
                    w0 = _cimg_lanczos(t + 2),
                    w1 = _cimg_lanczos(t + 1),
                    w2 = _cimg_lanczos(t),
                    w3 = _cimg_lanczos(t - 1),
                    w4 = _cimg_lanczos(t - 2),
                    val2 = (double)*ptrs,
                    val1 = ptrs>=ptrsmin?(double)*(ptrs - sx):val2,
                    val0 = ptrs>ptrsmin?(double)*(ptrs - 2*sx):val1,
                    val3 = ptrs<=ptrsmax?(double)*(ptrs + sx):val2,
                    val4 = ptrs<ptrsmax?(double)*(ptrs + 2*sx):val3,
                    val = (val0*w0 + val1*w1 + val2*w2 + val3*w3 + val4*w4)/(w1 + w2 + w3 + w4);
                  *ptrd = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrd+=sx;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resx.assign();
        } else resy.assign(resx,true);

        if (sz!=_depth) {
          if (_depth==1) resy.get_resize(sx,sy,sz,_spectrum,1).move_to(resz);
          else {
            if (_depth>sz) resy.get_resize(sx,sy,sz,_spectrum,2).move_to(resz);
            else {
              const double fz = (!boundary_conditions && sz>_depth)?(sz>1?(_depth - 1.0)/(sz - 1):0):
                (double)_depth/sz;
              const unsigned int sxy = sx*sy;
              resz.assign(sx,sy,sz,_spectrum);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forZ(resz,z) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(depth() - 1.0,curr + fz);
                *(poff++) = sxy*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resz.size()>=65536))
              cimg_forXYC(resz,x,y,c) {
                const T *const ptrs0 = resy.data(x,y,0,c), *ptrs = ptrs0, *const ptrsmin = ptrs0 + sxy,
                  *const ptrsmax = ptrs0 + (_depth - 2)*sxy;
                T *ptrd = resz.data(x,y,0,c);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forZ(resz,z) {
                  const double
                    t = *(pfoff++),
                    w0 = _cimg_lanczos(t + 2),
                    w1 = _cimg_lanczos(t + 1),
                    w2 = _cimg_lanczos(t),
                    w3 = _cimg_lanczos(t - 1),
                    w4 = _cimg_lanczos(t - 2),
                    val2 = (double)*ptrs,
                    val1 = ptrs>=ptrsmin?(double)*(ptrs - sxy):val2,
                    val0 = ptrs>ptrsmin?(double)*(ptrs - 2*sxy):val1,
                    val3 = ptrs<=ptrsmax?(double)*(ptrs + sxy):val2,
                    val4 = ptrs<ptrsmax?(double)*(ptrs + 2*sxy):val3,
                    val = (val0*w0 + val1*w1 + val2*w2 + val3*w3 + val4*w4)/(w1 + w2 + w3 + w4);
                  *ptrd = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrd+=sxy;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resy.assign();
        } else resz.assign(resy,true);

        if (sc!=_spectrum) {
          if (_spectrum==1) resz.get_resize(sx,sy,sz,sc,1).move_to(resc);
          else {
            if (_spectrum>sc) resz.get_resize(sx,sy,sz,sc,2).move_to(resc);
            else {
              const double fc = (!boundary_conditions && sc>_spectrum)?(sc>1?(_spectrum - 1.0)/(sc - 1):0):
                (double)_spectrum/sc;
              const unsigned int sxyz = sx*sy*sz;
              resc.assign(sx,sy,sz,sc);
              curr = old = 0;
              unsigned int *poff = off._data;
              double *pfoff = foff._data;
              cimg_forC(resc,c) {
                *(pfoff++) = curr - (unsigned int)curr;
                old = curr;
                curr = std::min(spectrum() - 1.0,curr + fc);
                *(poff++) = sxyz*((unsigned int)curr - (unsigned int)old);
              }
              cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(resc.size()>=65536))
              cimg_forXYZ(resc,x,y,z) {
                const T *const ptrs0 = resz.data(x,y,z,0), *ptrs = ptrs0, *const ptrsmin = ptrs0 + sxyz,
                  *const ptrsmax = ptrs + (_spectrum - 2)*sxyz;
                T *ptrd = resc.data(x,y,z,0);
                const unsigned int *poff = off._data;
                const double *pfoff = foff._data;
                cimg_forC(resc,c) {
                  const double
                    t = *(pfoff++),
                    w0 = _cimg_lanczos(t + 2),
                    w1 = _cimg_lanczos(t + 1),
                    w2 = _cimg_lanczos(t),
                    w3 = _cimg_lanczos(t - 1),
                    w4 = _cimg_lanczos(t - 2),
                    val2 = (double)*ptrs,
                    val1 = ptrs>=ptrsmin?(double)*(ptrs - sxyz):val2,
                    val0 = ptrs>ptrsmin?(double)*(ptrs - 2*sxyz):val1,
                    val3 = ptrs<=ptrsmax?(double)*(ptrs + sxyz):val2,
                    val4 = ptrs<ptrsmax?(double)*(ptrs + 2*sxyz):val3,
                    val = (val0*w0 + val1*w1 + val2*w2 + val3*w3 + val4*w4)/(w1 + w2 + w3 + w4);
                  *ptrd = (T)(val<vmin?vmin:val>vmax?vmax:val);
                  ptrd+=sxyz;
                  ptrs+=*(poff++);
                }
              }
            }
          }
          resz.assign();
        } else resc.assign(resz,true);

        return resc._is_shared?(resz._is_shared?(resy._is_shared?(resx._is_shared?(+(*this)):resx):resy):resz):resc;
      } break;

        // Unknow interpolation.
        //
      default :
        throw CImgArgumentException(_cimg_instance
                                    "resize(): Invalid specified interpolation %d "
                                    "(should be { -1=raw | 0=none | 1=nearest | 2=average | 3=linear | 4=grid | "
                                    "5=cubic | 6=lanczos }).",
                                    cimg_instance,
                                    interpolation_type);
      }
      return res;