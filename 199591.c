    **/
    CImg<T> rotate(const float u, const float v, const float w, const float angle,
                   const unsigned int interpolation, const unsigned int boundary_conditions) {
      const float nangle = cimg::mod(angle,360.0f);
      if (nangle==0.0f) return *this;
      return get_rotate(u,v,w,nangle,interpolation,boundary_conditions).move_to(*this);