
      static double mp_vector_print(_cimg_math_parser& mp) {
        const bool print_string = (bool)mp.opcode[4];
        cimg_pragma_openmp(critical(mp_vector_print))
        {
          CImg<charT> expr(mp.opcode[2] - 5);
          const ulongT *ptrs = mp.opcode._data + 5;
          cimg_for(expr,ptrd,char) *ptrd = (char)*(ptrs++);
          cimg::strellipsize(expr);
          unsigned int
            ptr = (unsigned int)mp.opcode[1] + 1,
            siz0 = (unsigned int)mp.opcode[3],
            siz = siz0;
          cimg::mutex(6);
          std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = [ ",expr._data);
          unsigned int count = 0;
          while (siz-->0) {
            if (count>=64 && siz>=64) {
              std::fprintf(cimg::output(),"...,");
              ptr = (unsigned int)mp.opcode[1] + 1 + siz0 - 64;
              siz = 64;
            } else std::fprintf(cimg::output(),"%g%s",mp.mem[ptr++],siz?",":"");
            ++count;
          }
          if (print_string) {
            CImg<charT> str(siz0 + 1);
            ptr = (unsigned int)mp.opcode[1] + 1;
            for (unsigned int k = 0; k<siz0; ++k) str[k] = (char)mp.mem[ptr++];
            str[siz0] = 0;
            cimg::strellipsize(str,1024,false);
            std::fprintf(cimg::output()," ] = '%s' (size: %u)",str._data,siz0);
          } else std::fprintf(cimg::output()," ] (size: %u)",siz0);
          std::fflush(cimg::output());
          cimg::mutex(6,0);
        }
        return cimg::type<double>::nan();