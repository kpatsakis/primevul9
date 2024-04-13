    template<typename t>
    CImg<doubleT> _eval(CImg<T> *const output, const char *const expression, const CImg<t>& xyzc,
                        const CImgList<T> *const list_inputs=0, CImgList<T> *const list_outputs=0) const {
      CImg<doubleT> res(1,xyzc.size()/4);
      if (!expression || !*expression) return res.fill(0);
      _cimg_math_parser mp(expression,"eval",*this,output,list_inputs,list_outputs,false);
#ifdef cimg_use_openmp
      cimg_pragma_openmp(parallel if (res._height>=512))
      {
        _cimg_math_parser
          _mp = omp_get_thread_num()?mp:_cimg_math_parser(),
          &lmp = omp_get_thread_num()?_mp:mp;
        cimg_pragma_openmp(for)
          for (unsigned int i = 0; i<res._height; ++i) {
            const unsigned int i4 = 4*i;
            const double
              x = (double)xyzc[i4], y = (double)xyzc[i4 + 1],
              z = (double)xyzc[i4 + 2], c = (double)xyzc[i4 + 3];
            res[i] = lmp(x,y,z,c);
          }
        }
#else
      const t *ps = xyzc._data;
      cimg_for(res,pd,double) {
        const double x = (double)*(ps++), y = (double)*(ps++), z = (double)*(ps++), c = (double)*(ps++);
        *pd = mp(x,y,z,c);
      }
#endif
      mp.end();
      return res;