    **/
    CImg<T>& displacement(const CImg<T>& source, const float smoothness=0.1f, const float precision=5.0f,
                          const unsigned int nb_scales=0, const unsigned int iteration_max=10000,
                          const bool is_backward=false,
                          const CImg<floatT>& guide=CImg<floatT>::const_empty()) {
      return get_displacement(source,smoothness,precision,nb_scales,iteration_max,is_backward,guide).
        move_to(*this);