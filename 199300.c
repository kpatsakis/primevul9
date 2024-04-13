
      static double mp_variance(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        CImg<doubleT> vals(i_end - 3);
        double *p = vals.data();
        for (unsigned int i = 3; i<i_end; ++i) *(p++) = _mp_arg(i);
        return vals.variance();