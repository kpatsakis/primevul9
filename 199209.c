      // Check a vector is not 0-dimensional, or with unknown dimension at compile time.
      void check_vector0(const unsigned int dim,
                         char *const ss, char *const se, const char saved_char) {
        char *s0 = 0;
        if (!dim) {
          *se = saved_char;
          s0 = ss - 4>expr._data?ss - 4:expr._data;
          cimg::strellipsize(s0,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Invalid construction of a 0-dimensional vector, "
                                      "in expression '%s%s%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s0!=expr._data?"...":"",s0,se<&expr.back()?"...":"");
        } else if (dim==~0U) {
          *se = saved_char;
          s0 = ss - 4>expr._data?ss - 4:expr._data;
          cimg::strellipsize(s0,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Invalid construction of a vector with possible dynamic size, "
                                      "in expression '%s%s%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s0!=expr._data?"...":"",s0,se<&expr.back()?"...":"");
        }