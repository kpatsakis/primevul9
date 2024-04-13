    template<typename t, typename to>
    static float __draw_object3d(const CImg<t>& opacities, const unsigned int n_primitive, CImg<to>& opacity) {
      opacity.assign();
      return n_primitive>=opacities._width?1.0f:(float)opacities[n_primitive];