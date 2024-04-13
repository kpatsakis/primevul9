
      static double mp_memcopy(_cimg_math_parser& mp) {
        longT siz = (longT)_mp_arg(4);
        const longT inc_d = (longT)_mp_arg(5), inc_s = (longT)_mp_arg(6);
        const float
          _opacity = (float)_mp_arg(7),
          opacity = (float)cimg::abs(_opacity),
          omopacity = 1 - std::max(_opacity,0.0f);
        if (siz>0) {
          const bool
            is_doubled = mp.opcode[8]<=1,
            is_doubles = mp.opcode[15]<=1;
          if (is_doubled && is_doubles) { // (double*) <- (double*)
            double *ptrd = _mp_memcopy_double(mp,(unsigned int)mp.opcode[2],&mp.opcode[8],siz,inc_d);
            const double *ptrs = _mp_memcopy_double(mp,(unsigned int)mp.opcode[3],&mp.opcode[15],siz,inc_s);
            if (inc_d==1 && inc_s==1 && _opacity>=1) {
              if (ptrs + siz - 1<ptrd || ptrs>ptrd + siz - 1) std::memcpy(ptrd,ptrs,siz*sizeof(double));
              else std::memmove(ptrd,ptrs,siz*sizeof(double));
            } else {
              if (ptrs + (siz - 1)*inc_s<ptrd || ptrs>ptrd + (siz - 1)*inc_d) {
                if (_opacity>=1) while (siz-->0) { *ptrd = *ptrs; ptrd+=inc_d; ptrs+=inc_s; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**ptrs; ptrd+=inc_d; ptrs+=inc_s; }
              } else { // Overlapping buffers
                CImg<doubleT> buf((unsigned int)siz);
                cimg_for(buf,ptr,double) { *ptr = *ptrs; ptrs+=inc_s; }
                ptrs = buf;
                if (_opacity>=1) while (siz-->0) { *ptrd = *(ptrs++); ptrd+=inc_d; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**(ptrs++); ptrd+=inc_d; }
              }
            }
          } else if (is_doubled && !is_doubles) { // (double*) <- (float*)
            double *ptrd = _mp_memcopy_double(mp,(unsigned int)mp.opcode[2],&mp.opcode[8],siz,inc_d);
            const float *ptrs = _mp_memcopy_float(mp,&mp.opcode[15],siz,inc_s);
            if (_opacity>=1) while (siz-->0) { *ptrd = *ptrs; ptrd+=inc_d; ptrs+=inc_s; }
            else while (siz-->0) { *ptrd = omopacity**ptrd + _opacity**ptrs; ptrd+=inc_d; ptrs+=inc_s; }
          } else if (!is_doubled && is_doubles) { // (float*) <- (double*)
            float *ptrd = _mp_memcopy_float(mp,&mp.opcode[8],siz,inc_d);
            const double *ptrs = _mp_memcopy_double(mp,(unsigned int)mp.opcode[3],&mp.opcode[15],siz,inc_s);
            if (_opacity>=1) while (siz-->0) { *ptrd = (float)*ptrs; ptrd+=inc_d; ptrs+=inc_s; }
            else while (siz-->0) { *ptrd = (float)(omopacity**ptrd + opacity**ptrs); ptrd+=inc_d; ptrs+=inc_s; }
          } else { // (float*) <- (float*)
            float *ptrd = _mp_memcopy_float(mp,&mp.opcode[8],siz,inc_d);
            const float *ptrs = _mp_memcopy_float(mp,&mp.opcode[15],siz,inc_s);
            if (inc_d==1 && inc_s==1 && _opacity>=1) {
              if (ptrs + siz - 1<ptrd || ptrs>ptrd + siz - 1) std::memcpy(ptrd,ptrs,siz*sizeof(float));
              else std::memmove(ptrd,ptrs,siz*sizeof(float));
            } else {
              if (ptrs + (siz - 1)*inc_s<ptrd || ptrs>ptrd + (siz - 1)*inc_d) {
                if (_opacity>=1) while (siz-->0) { *ptrd = *ptrs; ptrd+=inc_d; ptrs+=inc_s; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**ptrs; ptrd+=inc_d; ptrs+=inc_s; }
              } else { // Overlapping buffers
                CImg<floatT> buf((unsigned int)siz);
                cimg_for(buf,ptr,float) { *ptr = *ptrs; ptrs+=inc_s; }
                ptrs = buf;
                if (_opacity>=1) while (siz-->0) { *ptrd = *(ptrs++); ptrd+=inc_d; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**(ptrs++); ptrd+=inc_d; }
              }
            }
          }
        }
        return _mp_arg(1);