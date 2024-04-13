    template<typename t>
    CImg<_cimg_Ttfloat> get_correlate(const CImg<t>& kernel, const bool boundary_conditions=true,
                                      const bool is_normalized=false) const {
      return _correlate(kernel,boundary_conditions,is_normalized,false);