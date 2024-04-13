      // Check a matrix is square.
      void check_matrix_square(const unsigned int arg, const unsigned int n_arg,
                               char *const ss, char *const se, const char saved_char) {
        _cimg_mp_check_type(arg,n_arg,2,0);
        const unsigned int
          siz = _cimg_mp_size(arg),
          n = (unsigned int)std::sqrt((float)siz);
        if (n*n!=siz) {
          const char *s_arg;
          if (*s_op!='F') s_arg = !n_arg?"":n_arg==1?"Left-hand ":"Right-hand ";
          else s_arg = !n_arg?"":n_arg==1?"First ":n_arg==2?"Second ":n_arg==3?"Third ":"One ";
          *se = saved_char;
          char *const s0 = ss - 4>expr._data?ss - 4:expr._data;
          cimg::strellipsize(s0,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s (of type '%s') "
                                      "cannot be considered as a square matrix, in expression '%s%s%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_op=='F'?(*s_arg?"argument":"Argument"):(*s_arg?"operand":"Operand"),
                                      s_type(arg)._data,
                                      s0!=expr._data?"...":"",s0,se<&expr.back()?"...":"");
        }