
      static double mp_normp(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        if (i_end==4) return cimg::abs(_mp_arg(3));
        const double p = (double)mp.opcode[3];
        double res = 0;
        for (unsigned int i = 4; i<i_end; ++i)
          res+=std::pow(cimg::abs(_mp_arg(i)),p);
        res = std::pow(res,1/p);
        return res>0?res:0.0;