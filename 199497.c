    template<typename t>
    static float ___draw_object3d(const CImgList<t>& opacities, const unsigned int n_primitive) {
      return n_primitive<opacities._width && opacities[n_primitive].size()==1?(float)opacities(n_primitive,0):1.0f;