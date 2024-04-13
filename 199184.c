
      static double mp_self_map_vector_s(_cimg_math_parser& mp) { // Vector += scalar
        unsigned int
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[2];
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,3);
        l_opcode[2] = mp.opcode[4]; // Scalar argument.
        l_opcode.swap(mp.opcode);
        ulongT &target = mp.opcode[1];
        while (siz-->0) { target = ptrd++; (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();