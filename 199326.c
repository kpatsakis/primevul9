
      static double mp_isbool(_cimg_math_parser& mp) {
        const double val = _mp_arg(2);
        return (double)(val==0.0 || val==1.0);