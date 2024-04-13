    template<typename tp, typename tc, typename to>
    bool is_object3d(const CImgList<tp>& primitives,
                     const CImgList<tc>& colors,
                     const to& opacities,
                     const bool full_check=true,
                     char *const error_message=0) const {
      if (error_message) *error_message = 0;

      // Check consistency for the particular case of an empty 3d object.
      if (is_empty()) {
        if (primitives || colors || opacities) {
          if (error_message) cimg_sprintf(error_message,
                                          "3d object (%u,%u) defines no vertices but %u primitives, "
                                          "%u colors and %lu opacities",
                                          _width,primitives._width,primitives._width,
                                          colors._width,(unsigned long)opacities.size());
          return false;
        }
        return true;
      }

      // Check consistency of vertices.
      if (_height!=3 || _depth>1 || _spectrum>1) { // Check vertices dimensions.
        if (error_message) cimg_sprintf(error_message,
                                        "3d object (%u,%u) has invalid vertex dimensions (%u,%u,%u,%u)",
                                        _width,primitives._width,_width,_height,_depth,_spectrum);
        return false;
      }
      if (colors._width>primitives._width + 1) {
        if (error_message) cimg_sprintf(error_message,
                                        "3d object (%u,%u) defines %u colors",
                                        _width,primitives._width,colors._width);
        return false;
      }
      if (opacities.size()>primitives._width) {
        if (error_message) cimg_sprintf(error_message,
                                        "3d object (%u,%u) defines %lu opacities",
                                        _width,primitives._width,(unsigned long)opacities.size());
        return false;
      }
      if (!full_check) return true;

      // Check consistency of primitives.
      cimglist_for(primitives,l) {
        const CImg<tp>& primitive = primitives[l];
        const unsigned int psiz = (unsigned int)primitive.size();
        switch (psiz) {
        case 1 : { // Point.
          const unsigned int i0 = (unsigned int)primitive(0);
          if (i0>=_width) {
            if (error_message) cimg_sprintf(error_message,
                                            "3d object (%u,%u) refers to invalid vertex indice %u in "
                                            "point primitive [%u]",
                                            _width,primitives._width,i0,l);
            return false;
          }
        } break;
        case 5 : { // Sphere.
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          if (i0>=_width || i1>=_width) {
            if (error_message) cimg_sprintf(error_message,
                                            "3d object (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "sphere primitive [%u]",
                                            _width,primitives._width,i0,i1,l);
            return false;
          }
        } break;
        case 2 : // Segment.
        case 6 : {
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          if (i0>=_width || i1>=_width) {
            if (error_message) cimg_sprintf(error_message,
                                            "3d object (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "segment primitive [%u]",
                                            _width,primitives._width,i0,i1,l);
            return false;
          }
        } break;
        case 3 : // Triangle.
        case 9 : {
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2);
          if (i0>=_width || i1>=_width || i2>=_width) {
            if (error_message) cimg_sprintf(error_message,
                                            "3d object (%u,%u) refers to invalid vertex indices (%u,%u,%u) in "
                                            "triangle primitive [%u]",
                                            _width,primitives._width,i0,i1,i2,l);
            return false;
          }
        } break;
        case 4 : // Quadrangle.
        case 12 : {
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2),
            i3 = (unsigned int)primitive(3);
          if (i0>=_width || i1>=_width || i2>=_width || i3>=_width) {
            if (error_message) cimg_sprintf(error_message,
                                            "3d object (%u,%u) refers to invalid vertex indices (%u,%u,%u,%u) in "
                                            "quadrangle primitive [%u]",
                                            _width,primitives._width,i0,i1,i2,i3,l);
            return false;
          }
        } break;
        default :
          if (error_message) cimg_sprintf(error_message,
                                          "3d object (%u,%u) defines an invalid primitive [%u] of size %u",
                                          _width,primitives._width,l,(unsigned int)psiz);
          return false;
        }
      }

      // Check consistency of colors.
      cimglist_for(colors,c) {
        const CImg<tc>& color = colors[c];
        if (!color) {
          if (error_message) cimg_sprintf(error_message,
                                          "3d object (%u,%u) defines no color for primitive [%u]",
                                          _width,primitives._width,c);
          return false;
        }
      }

      // Check consistency of light texture.
      if (colors._width>primitives._width) {
        const CImg<tc> &light = colors.back();
        if (!light || light._depth>1) {
          if (error_message) cimg_sprintf(error_message,
                                          "3d object (%u,%u) defines an invalid light texture (%u,%u,%u,%u)",
                                          _width,primitives._width,light._width,
                                          light._height,light._depth,light._spectrum);
          return false;
        }
      }

      return true;