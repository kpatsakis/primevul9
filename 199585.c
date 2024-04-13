
      static double mp_isin(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        const double val = _mp_arg(3);
        for (unsigned int i = 4; i<i_end; ++i)
          if (val==_mp_arg(i)) return 1.0;
        return 0.0;