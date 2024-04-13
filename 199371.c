    **/
    CImg<T> rotate(const float u, const float v, const float w, const float angle,
                   const float cx, const float cy, const float cz,
                   const unsigned int interpolation=1, const unsigned int boundary_conditions=0) {
      const float nangle = cimg::mod(angle,360.0f);
      if (nangle==0.0f) return *this;
      return get_rotate(u,v,w,nangle,cx,cy,cz,interpolation,boundary_conditions).move_to(*this);