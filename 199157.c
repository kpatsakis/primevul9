    template<typename t, typename to>
    static float __draw_object3d(const CImgList<t>& opacities, const unsigned int n_primitive, CImg<to>& opacity) {
      if (n_primitive>=opacities._width || opacities[n_primitive].is_empty()) { opacity.assign(); return 1; }
      if (opacities[n_primitive].size()==1) { opacity.assign(); return opacities(n_primitive,0); }
      opacity.assign(opacities[n_primitive],true);
      return 1.0f;