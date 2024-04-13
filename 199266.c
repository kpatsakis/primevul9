    template<typename t>
    CImg<T>& warp(const CImg<t>& warp, const unsigned int mode=0,
                  const unsigned int interpolation=1, const unsigned int boundary_conditions=0) {
      return get_warp(warp,mode,interpolation,boundary_conditions).move_to(*this);