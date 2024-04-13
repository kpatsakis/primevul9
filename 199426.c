
      static double mp_matrix_svd(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptr1 = &_mp_arg(2) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[3],
          l = (unsigned int)mp.opcode[4];
        CImg<doubleT> U, S, V;
        CImg<doubleT>(ptr1,k,l,1,1,true).SVD(U,S,V);
        CImg<doubleT>(ptrd,k,l,1,1,true) = U;
        CImg<doubleT>(ptrd + k*l,1,k,1,1,true) = S;
        CImg<doubleT>(ptrd + k*l + k,k,k,1,1,true) = V;
        return cimg::type<double>::nan();