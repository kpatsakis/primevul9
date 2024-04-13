    **/
    CImg<T>& rotate(const float angle, const unsigned int interpolation=1,
                    const unsigned int boundary_conditions=0) {
      const float nangle = cimg::mod(angle,360.0f);
      if (nangle==0.0f) return *this;
      return get_rotate(nangle,interpolation,boundary_conditions).move_to(*this);