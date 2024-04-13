
      static double mp_diag(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        CImg<doubleT>(ptrd,k,k,1,1,true) = CImg<doubleT>(ptrs,1,k,1,1,true).get_diagonal();
        return cimg::type<double>::nan();