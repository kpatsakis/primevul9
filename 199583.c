
    CImg<T>& _fill(const char *const expression, const bool repeat_values, bool allow_formula,
                   const CImgList<T> *const list_inputs, CImgList<T> *const list_outputs,
                   const char *const calling_function, const CImg<T> *provides_copy) {
      if (is_empty() || !expression || !*expression) return *this;
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      CImg<charT> is_error;
      bool is_value_sequence = false;
      cimg_abort_init;

      if (allow_formula) {

        // Try to pre-detect regular value sequence to avoid exception thrown by _cimg_math_parser.
        double value;
        char sep;
        const int err = cimg_sscanf(expression,"%lf %c",&value,&sep);
        if (err==1 || (err==2 && sep==',')) {
          if (err==1) return fill((T)value);
          else is_value_sequence = true;
        }

        // Try to fill values according to a formula.
        _cimg_abort_init_omp;
        if (!is_value_sequence) try {
            CImg<T> base = provides_copy?provides_copy->get_shared():get_shared();
            _cimg_math_parser mp(expression + (*expression=='>' || *expression=='<' ||
                                               *expression=='*' || *expression==':'),
                                 calling_function,base,this,list_inputs,list_outputs,true);
            if (!provides_copy && expression && *expression!='>' && *expression!='<' && *expression!=':' &&
                mp.need_input_copy)
              base.assign().assign(*this,false); // Needs input copy

            bool do_in_parallel = false;
#ifdef cimg_use_openmp
            cimg_openmp_if(*expression=='*' || *expression==':' ||
                           (mp.is_parallelizable && _width>=320 && _height*_depth*_spectrum>=2))
              do_in_parallel = true;
#endif
            if (mp.result_dim) { // Vector-valued expression
              const unsigned int N = std::min(mp.result_dim,_spectrum);
              const ulongT whd = (ulongT)_width*_height*_depth;
              T *ptrd = *expression=='<'?_data + _width*_height*_depth - 1:_data;
              if (*expression=='<') {
                CImg<doubleT> res(1,mp.result_dim);
                cimg_rofYZ(*this,y,z) {
                  cimg_abort_test;
                  cimg_rofX(*this,x) {
                    mp(x,y,z,0,res._data);
                    const double *ptrs = res._data;
                    T *_ptrd = ptrd--; for (unsigned int n = N; n>0; --n) { *_ptrd = (T)(*ptrs++); _ptrd+=whd; }
                  }
                }
              } else if (*expression=='>' || !do_in_parallel) {
                CImg<doubleT> res(1,mp.result_dim);
                cimg_forYZ(*this,y,z) {
                  cimg_abort_test;
                  cimg_forX(*this,x) {
                    mp(x,y,z,0,res._data);
                    const double *ptrs = res._data;
                    T *_ptrd = ptrd++; for (unsigned int n = N; n>0; --n) { *_ptrd = (T)(*ptrs++); _ptrd+=whd; }
                  }
                }
             } else {
#ifdef cimg_use_openmp
                cimg_pragma_openmp(parallel)
                {
                  _cimg_math_parser
                    _mp = omp_get_thread_num()?mp:_cimg_math_parser(),
                    &lmp = omp_get_thread_num()?_mp:mp;
                  lmp.is_fill = true;
                  cimg_pragma_openmp(for collapse(2))
                    cimg_forYZ(*this,y,z) _cimg_abort_try_omp {
                    cimg_abort_test;
                    CImg<doubleT> res(1,lmp.result_dim);
                    T *ptrd = data(0,y,z,0);
                    cimg_forX(*this,x) {
                      lmp(x,y,z,0,res._data);
                      const double *ptrs = res._data;
                      T *_ptrd = ptrd++; for (unsigned int n = N; n>0; --n) { *_ptrd = (T)(*ptrs++); _ptrd+=whd; }
                    }
                  } _cimg_abort_catch_omp _cimg_abort_catch_fill_omp
                }
#endif
              }

            } else { // Scalar-valued expression
              T *ptrd = *expression=='<'?end() - 1:_data;
              if (*expression=='<')
                cimg_rofYZC(*this,y,z,c) { cimg_abort_test; cimg_rofX(*this,x) *(ptrd--) = (T)mp(x,y,z,c); }
              else if (*expression=='>' || !do_in_parallel)
                cimg_forYZC(*this,y,z,c) { cimg_abort_test; cimg_forX(*this,x) *(ptrd++) = (T)mp(x,y,z,c); }
              else {
#ifdef cimg_use_openmp
                cimg_pragma_openmp(parallel)
                {
                  _cimg_math_parser
                    _mp = omp_get_thread_num()?mp:_cimg_math_parser(),
                    &lmp = omp_get_thread_num()?_mp:mp;
                  lmp.is_fill = true;
                  cimg_pragma_openmp(for collapse(3))
                    cimg_forYZC(*this,y,z,c) _cimg_abort_try_omp {
                    cimg_abort_test;
                    T *ptrd = data(0,y,z,c);
                    cimg_forX(*this,x) *ptrd++ = (T)lmp(x,y,z,c);
                  } _cimg_abort_catch_omp _cimg_abort_catch_fill_omp
                }
#endif
              }
            }
            mp.end();
          } catch (CImgException& e) { CImg<charT>::string(e._message).move_to(is_error); }
      }

      // Try to fill values according to a value sequence.
      if (!allow_formula || is_value_sequence || is_error) {
        CImg<charT> item(256);
        char sep = 0;
        const char *nexpression = expression;
        ulongT nb = 0;
        const ulongT siz = size();
        T *ptrd = _data;
        for (double val = 0; *nexpression && nb<siz; ++nb) {
          sep = 0;
          const int err = cimg_sscanf(nexpression,"%255[ \n\t0-9.eEinfa+-]%c",item._data,&sep);
          if (err>0 && cimg_sscanf(item,"%lf",&val)==1 && (sep==',' || sep==';' || err==1)) {
            nexpression+=std::strlen(item) + (err>1);
            *(ptrd++) = (T)val;
          } else break;
        }
        cimg::exception_mode(omode);
        if (nb<siz && (sep || *nexpression)) {
          if (is_error) throw CImgArgumentException("%s",is_error._data);
          else throw CImgArgumentException(_cimg_instance
                                           "%s(): Invalid sequence of filling values '%s'.",
                                           cimg_instance,calling_function,expression);
        }
        if (repeat_values && nb && nb<siz)
          for (T *ptrs = _data, *const ptre = _data + siz; ptrd<ptre; ++ptrs) *(ptrd++) = *ptrs;
      }

      cimg::exception_mode(omode);
      cimg_abort_test;
      return *this;