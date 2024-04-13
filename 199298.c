
      static double mp_argkth(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        const double val = mp_kth(mp);
        for (unsigned int i = 4; i<i_end; ++i) if (val==_mp_arg(i)) return i - 3.0;
        return 1;