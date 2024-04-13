    **/
    bool is_CImg3d(const bool full_check=true, char *const error_message=0) const {
      if (error_message) *error_message = 0;

      // Check instance dimension and header.
      if (_width!=1 || _height<8 || _depth!=1 || _spectrum!=1) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d has invalid dimensions (%u,%u,%u,%u)",
                                        _width,_height,_depth,_spectrum);
        return false;
      }
      const T *ptrs = _data, *const ptre = end();
      if (!_is_CImg3d(*(ptrs++),'C') || !_is_CImg3d(*(ptrs++),'I') || !_is_CImg3d(*(ptrs++),'m') ||
          !_is_CImg3d(*(ptrs++),'g') || !_is_CImg3d(*(ptrs++),'3') || !_is_CImg3d(*(ptrs++),'d')) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d header not found");
        return false;
      }
      const unsigned int
        nb_points = cimg::float2uint((float)*(ptrs++)),
        nb_primitives = cimg::float2uint((float)*(ptrs++));

      // Check consistency of number of vertices / primitives.
      if (!full_check) {
        const ulongT minimal_size = 8UL + 3*nb_points + 6*nb_primitives;
        if (_data + minimal_size>ptre) {
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) has only %lu values, while at least %lu values were expected",
                                          nb_points,nb_primitives,(unsigned long)size(),(unsigned long)minimal_size);
          return false;
        }
      }

      // Check consistency of vertex data.
      if (!nb_points) {
        if (nb_primitives) {
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) defines no vertices but %u primitives",
                                          nb_points,nb_primitives,nb_primitives);
          return false;
        }
        if (ptrs!=ptre) {
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) is an empty object but contains %u value%s "
                                          "more than expected",
                                          nb_points,nb_primitives,(unsigned int)(ptre - ptrs),(ptre - ptrs)>1?"s":"");
          return false;
        }
        return true;
      }
      if (ptrs + 3*nb_points>ptre) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d (%u,%u) defines only %u vertices data",
                                        nb_points,nb_primitives,(unsigned int)(ptre - ptrs)/3);
        return false;
      }
      ptrs+=3*nb_points;

      // Check consistency of primitive data.
      if (ptrs==ptre) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d (%u,%u) defines %u vertices but no primitive",
                                        nb_points,nb_primitives,nb_points);
        return false;
      }

      if (!full_check) return true;

      for (unsigned int p = 0; p<nb_primitives; ++p) {
        const unsigned int nb_inds = (unsigned int)*(ptrs++);
        switch (nb_inds) {
        case 1 : { // Point.
          const unsigned int i0 = cimg::float2uint((float)*(ptrs++));
          if (i0>=nb_points) {
            if (error_message) cimg_sprintf(error_message,
                                            "CImg3d (%u,%u) refers to invalid vertex indice %u in point primitive [%u]",
                                            nb_points,nb_primitives,i0,p);
            return false;
          }
        } break;
        case 5 : { // Sphere.
          const unsigned int
            i0 = cimg::float2uint((float)*(ptrs++)),
            i1 = cimg::float2uint((float)*(ptrs++));
          ptrs+=3;
          if (i0>=nb_points || i1>=nb_points) {
            if (error_message) cimg_sprintf(error_message,
                                            "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "sphere primitive [%u]",
                                            nb_points,nb_primitives,i0,i1,p);
            return false;
          }
        } break;
        case 2 : case 6 : { // Segment.
          const unsigned int
            i0 = cimg::float2uint((float)*(ptrs++)),
            i1 = cimg::float2uint((float)*(ptrs++));
          if (nb_inds==6) ptrs+=4;
          if (i0>=nb_points || i1>=nb_points) {
            if (error_message) cimg_sprintf(error_message,
                                            "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "segment primitive [%u]",
                                            nb_points,nb_primitives,i0,i1,p);
            return false;
          }
        } break;
        case 3 : case 9 : { // Triangle.
          const unsigned int
            i0 = cimg::float2uint((float)*(ptrs++)),
            i1 = cimg::float2uint((float)*(ptrs++)),
            i2 = cimg::float2uint((float)*(ptrs++));
          if (nb_inds==9) ptrs+=6;
          if (i0>=nb_points || i1>=nb_points || i2>=nb_points) {
            if (error_message) cimg_sprintf(error_message,
                                            "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u,%u) in "
                                            "triangle primitive [%u]",
                                            nb_points,nb_primitives,i0,i1,i2,p);
            return false;
          }
        } break;
        case 4 : case 12 : { // Quadrangle.
          const unsigned int
            i0 = cimg::float2uint((float)*(ptrs++)),
            i1 = cimg::float2uint((float)*(ptrs++)),
            i2 = cimg::float2uint((float)*(ptrs++)),
            i3 = cimg::float2uint((float)*(ptrs++));
          if (nb_inds==12) ptrs+=8;
          if (i0>=nb_points || i1>=nb_points || i2>=nb_points || i3>=nb_points) {
            if (error_message) cimg_sprintf(error_message,
                                            "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u,%u,%u) in "
                                            "quadrangle primitive [%u]",
                                            nb_points,nb_primitives,i0,i1,i2,i3,p);
            return false;
          }
        } break;
        default :
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) defines an invalid primitive [%u] of size %u",
                                          nb_points,nb_primitives,p,nb_inds);
          return false;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) has incomplete primitive data for primitive [%u], "
                                          "%u values missing",
                                          nb_points,nb_primitives,p,(unsigned int)(ptrs - ptre));
          return false;
        }
      }

      // Check consistency of color data.
      if (ptrs==ptre) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d (%u,%u) defines no color/texture data",
                                        nb_points,nb_primitives);
        return false;
      }
      for (unsigned int c = 0; c<nb_primitives; ++c) {
        if (*(ptrs++)!=(T)-128) ptrs+=2;
        else if ((ptrs+=3)<ptre) {
          const unsigned int
            w = (unsigned int)*(ptrs - 3),
            h = (unsigned int)*(ptrs - 2),
            s = (unsigned int)*(ptrs - 1);
          if (!h && !s) {
            if (w>=c) {
              if (error_message) cimg_sprintf(error_message,
                                              "CImg3d (%u,%u) refers to invalid shared sprite/texture indice %u "
                                              "for primitive [%u]",
                                              nb_points,nb_primitives,w,c);
              return false;
            }
          } else ptrs+=w*h*s;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) has incomplete color/texture data for primitive [%u], "
                                          "%u values missing",
                                          nb_points,nb_primitives,c,(unsigned int)(ptrs - ptre));
          return false;
        }
      }

      // Check consistency of opacity data.
      if (ptrs==ptre) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d (%u,%u) defines no opacity data",
                                        nb_points,nb_primitives);
        return false;
      }
      for (unsigned int o = 0; o<nb_primitives; ++o) {
        if (*(ptrs++)==(T)-128 && (ptrs+=3)<ptre) {
          const unsigned int
            w = (unsigned int)*(ptrs - 3),
            h = (unsigned int)*(ptrs - 2),
            s = (unsigned int)*(ptrs - 1);
          if (!h && !s) {
            if (w>=o) {
              if (error_message) cimg_sprintf(error_message,
                                              "CImg3d (%u,%u) refers to invalid shared opacity indice %u "
                                              "for primitive [%u]",
                                              nb_points,nb_primitives,w,o);
              return false;
            }
          } else ptrs+=w*h*s;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_sprintf(error_message,
                                          "CImg3d (%u,%u) has incomplete opacity data for primitive [%u]",
                                          nb_points,nb_primitives,o);
          return false;
        }
      }

      // Check end of data.
      if (ptrs<ptre) {
        if (error_message) cimg_sprintf(error_message,
                                        "CImg3d (%u,%u) contains %u value%s more than expected",
                                        nb_points,nb_primitives,(unsigned int)(ptre - ptrs),(ptre - ptrs)>1?"s":"");
        return false;
      }
      return true;