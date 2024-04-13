
      static double mp_stov(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2);
        const ulongT siz = (ulongT)mp.opcode[3];
        longT ind = (longT)_mp_arg(4);
        const bool is_strict = (bool)_mp_arg(5);
        double val = cimg::type<double>::nan();
        if (ind<0 || ind>=(longT)siz) return val;
        if (!siz) return *ptrs>='0' && *ptrs<='9'?*ptrs - '0':val;

        CImg<charT> ss(siz + 1 - ind);
        char sep;
        ptrs+=1 + ind; cimg_forX(ss,i) ss[i] = (char)*(ptrs++); ss.back() = 0;

        int err = std::sscanf(ss,"%lf%c",&val,&sep);
#if cimg_OS==2
        // Check for +/-NaN and +/-inf as Microsoft's sscanf() version is not able
        // to read those particular values.
        if (!err && (*ss=='+' || *ss=='-' || *ss=='i' || *ss=='I' || *ss=='n' || *ss=='N')) {
          bool is_positive = true;
          const char *s = ss;
          if (*s=='+') ++s; else if (*s=='-') { ++s; is_positive = false; }
          if (!cimg::strcasecmp(s,"inf")) { val = cimg::type<double>::inf(); err = 1; }
          else if (!cimg::strcasecmp(s,"nan")) { val = cimg::type<double>::nan(); err = 1; }
          if (err==1 && !is_positive) val = -val;
        }
#endif
        if (is_strict && err!=1) return cimg::type<double>::nan();
        return val;