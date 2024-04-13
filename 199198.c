    **/
    CImg<Tdouble> get_stats(const unsigned int variance_method=1) const {
      if (is_empty()) return CImg<doubleT>();
      const T *const p_end = end(), *pm = _data, *pM = _data;
      double S = 0, S2 = 0, P = 1;
      const ulongT siz = size();
      T m = *pm, M = *pM;

      cimg_pragma_openmp(parallel reduction(+:S,S2) reduction(*:P) cimg_openmp_if(siz>=131072)) {
        const T *lpm = _data, *lpM = _data;
        T lm = *lpm, lM = *lpM;
        cimg_pragma_openmp(for)
        for (const T *ptrs = _data; ptrs<p_end; ++ptrs) {
          const T val = *ptrs;
          const double _val = (double)val;
          if (val<lm) { lm = val; lpm = ptrs; }
          if (val>lM) { lM = val; lpM = ptrs; }
          S+=_val;
          S2+=_val*_val;
          P*=_val;
        }
        cimg_pragma_openmp(critical(get_stats)) {
          if (lm<m || (lm==m && lpm<pm)) { m = lm; pm = lpm; }
          if (lM>M || (lM==M && lpM<pM)) { M = lM; pM = lpM; }
        }
      }

      const double
        mean_value = S/siz,
        _variance_value = variance_method==0?(S2 - S*S/siz)/siz:
        (variance_method==1?(siz>1?(S2 - S*S/siz)/(siz - 1):0):
         variance(variance_method)),
        variance_value = _variance_value>0?_variance_value:0;
      int
        xm = 0, ym = 0, zm = 0, cm = 0,
        xM = 0, yM = 0, zM = 0, cM = 0;
      contains(*pm,xm,ym,zm,cm);
      contains(*pM,xM,yM,zM,cM);
      return CImg<Tdouble>(1,14).fill((double)m,(double)M,mean_value,variance_value,
                                      (double)xm,(double)ym,(double)zm,(double)cm,
                                      (double)xM,(double)yM,(double)zM,(double)cM,
                                      S,P);