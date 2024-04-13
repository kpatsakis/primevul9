
      static double mp_fsize(_cimg_math_parser& mp) {
        const CImg<charT> filename(mp.opcode._data + 3,mp.opcode[2] - 3);
        return (double)cimg::fsize(filename);