    template<typename t>
    CImg<T>& convolve(const CImg<t>& kernel, const bool boundary_conditions=true, const bool is_normalized=false) {
      if (is_empty() || !kernel) return *this;
      return get_convolve(kernel,boundary_conditions,is_normalized).move_to(*this);