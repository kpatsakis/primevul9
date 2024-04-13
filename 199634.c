    template<typename tp, typename tf, typename tc, typename to>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors, const CImgList<to>& opacities,
                           const unsigned int render_type=4,
                           const bool is_double_sided=false, const float focale=700,
                           const float lightx=0, const float lighty=0, const float lightz=-5e8,
                           const float specular_lightness=0.2f, const float specular_shininess=0.1f) {
      return draw_object3d(x0,y0,z0,vertices,primitives,colors,opacities,render_type,
                           is_double_sided,focale,lightx,lighty,lightz,
                           specular_lightness,specular_shininess,CImg<floatT>::empty());