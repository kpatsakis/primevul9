    **/
    CImg<T>& equalize(const unsigned int nb_levels, const T& min_value, const T& max_value) {
      if (!nb_levels || is_empty()) return *this;
      const T
        vmin = min_value<max_value?min_value:max_value,
        vmax = min_value<max_value?max_value:min_value;
      CImg<ulongT> hist = get_histogram(nb_levels,vmin,vmax);
      ulongT cumul = 0;
      cimg_forX(hist,pos) { cumul+=hist[pos]; hist[pos] = cumul; }
      if (!cumul) cumul = 1;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=1048576))
      cimg_rof(*this,ptrd,T) {
        const int pos = (int)((*ptrd-vmin)*(nb_levels - 1.)/(vmax-vmin));
        if (pos>=0 && pos<(int)nb_levels) *ptrd = (T)(vmin + (vmax-vmin)*hist[pos]/cumul);
      }
      return *this;