    template<typename tp, typename tf, typename tc, typename to, typename tz>
    CImg<T>& draw_object3d(LibBoard::Board& board,
                           const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors,
                           const unsigned int render_type,
                           const bool is_double_sided, const float focale,
                           const float lightx, const float lighty, const float lightz,
                           const float specular_lightness, const float specular_shininess,
                           CImg<tz>& zbuffer) {
      return draw_object3d(x0,y0,z0,vertices,primitives,colors,CImg<floatT>::const_empty(),
                           render_type,is_double_sided,focale,lightx,lighty,lightz,
                           specular_lightness,specular_shininess,zbuffer);