
      static double mp_debug(_cimg_math_parser& mp) {
        CImg<charT> expr(mp.opcode[2] - 4);
        const ulongT *ptrs = mp.opcode._data + 4;
        cimg_for(expr,ptrd,char) *ptrd = (char)*(ptrs++);
        cimg::strellipsize(expr);
        const ulongT g_target = mp.opcode[1];

#ifndef cimg_use_openmp
        const unsigned int n_thread = 0;
#else
        const unsigned int n_thread = omp_get_thread_num();
#endif
        cimg_pragma_openmp(critical(mp_debug))
        {
          std::fprintf(cimg::output(),
                       "\n[" cimg_appname "_math_parser] %p[thread #%u]:%*c"
                       "Start debugging expression '%s', code length %u -> mem[%u] (memsize: %u)",
                       (void*)&mp,n_thread,mp.debug_indent,' ',
                       expr._data,(unsigned int)mp.opcode[3],(unsigned int)g_target,mp.mem._width);
          std::fflush(cimg::output());
          mp.debug_indent+=3;
        }
        const CImg<ulongT> *const p_end = (++mp.p_code) + mp.opcode[3];
        CImg<ulongT> _op;
        for ( ; mp.p_code<p_end; ++mp.p_code) {
          const CImg<ulongT> &op = *mp.p_code;
          mp.opcode._data = op._data;

          _op.assign(1,op._height - 1);
          const ulongT *ptrs = op._data + 1;
          for (ulongT *ptrd = _op._data, *const ptrde = _op._data + _op._height; ptrd<ptrde; ++ptrd)
            *ptrd = *(ptrs++);

          const ulongT target = mp.opcode[1];
          mp.mem[target] = _cimg_mp_defunc(mp);
          cimg_pragma_openmp(critical(mp_debug))
          {
            std::fprintf(cimg::output(),
                         "\n[" cimg_appname "_math_parser] %p[thread #%u]:%*c"
                         "Opcode %p = [ %p,%s ] -> mem[%u] = %g",
                         (void*)&mp,n_thread,mp.debug_indent,' ',
                         (void*)mp.opcode._data,(void*)*mp.opcode,_op.value_string().data(),
                         (unsigned int)target,mp.mem[target]);
            std::fflush(cimg::output());
          }
        }
        cimg_pragma_openmp(critical(mp_debug))
        {
          mp.debug_indent-=3;
          std::fprintf(cimg::output(),
            "\n[" cimg_appname "_math_parser] %p[thread #%u]:%*c"
            "End debugging expression '%s' -> mem[%u] = %g (memsize: %u)",
            (void*)&mp,n_thread,mp.debug_indent,' ',
            expr._data,(unsigned int)g_target,mp.mem[g_target],mp.mem._width);
          std::fflush(cimg::output());
        }
        --mp.p_code;
        return mp.mem[g_target];