
      static double mp_date(_cimg_math_parser& mp) {
        const unsigned int
          _arg = (unsigned int)mp.opcode[3],
          _siz = (unsigned int)mp.opcode[4],
          siz = _siz?_siz:1;
        const double *const arg_in = _arg==~0U?0:&_mp_arg(3) + (_siz?1:0);
        double *const arg_out = &_mp_arg(1) + (_siz?1:0);
        if (arg_in) std::memcpy(arg_out,arg_in,siz*sizeof(double));
        else for (unsigned int i = 0; i<siz; ++i) arg_out[i] = i;

        CImg<charT> filename(mp.opcode[2] - 5);
        if (filename) {
          const ulongT *ptrs = mp.opcode._data + 5;
          cimg_for(filename,ptrd,char) *ptrd = (char)*(ptrs++);
          cimg::fdate(filename,arg_out,siz);
        } else cimg::date(arg_out,siz);
        return _siz?cimg::type<double>::nan():*arg_out;