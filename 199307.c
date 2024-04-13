
      static double mp_vector_crop(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const longT
          length = (longT)mp.opcode[3],
          start = (longT)_mp_arg(4),
          sublength = (longT)mp.opcode[5];
        if (start<0 || start + sublength>length)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Value accessor '[]': "
                                      "Out-of-bounds sub-vector request "
                                      "(length: %ld, start: %ld, sub-length: %ld).",
                                      mp.imgin.pixel_type(),length,start,sublength);
        std::memcpy(ptrd,ptrs + start,sublength*sizeof(double));
        return cimg::type<double>::nan();