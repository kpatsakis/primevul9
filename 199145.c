    template<typename t>
    static float ___draw_object3d(const CImg<t>& opacities, const unsigned int n_primitive) {
      return n_primitive<opacities._width?(float)opacities[n_primitive]:1.0f;