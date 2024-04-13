    **/
    CImg<T>& quantize(const unsigned int nb_levels, const bool keep_range=true) {
      if (!nb_levels)
        throw CImgArgumentException(_cimg_instance
                                    "quantize(): Invalid quantization request with 0 values.",
                                    cimg_instance);

      if (is_empty()) return *this;
      Tfloat m, M = (Tfloat)max_min(m), range = M - m;
      if (range>0) {
        if (keep_range)
          cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
          cimg_rof(*this,ptrd,T) {
            const unsigned int val = (unsigned int)((*ptrd-m)*nb_levels/range);
            *ptrd = (T)(m + std::min(val,nb_levels - 1)*range/nb_levels);
          } else
          cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
          cimg_rof(*this,ptrd,T) {
            const unsigned int val = (unsigned int)((*ptrd-m)*nb_levels/range);
            *ptrd = (T)std::min(val,nb_levels - 1);
          }
      }
      return *this;