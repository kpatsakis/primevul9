
      _cimg_math_parser(const _cimg_math_parser& mp):
        mem(mp.mem),code(mp.code),p_code_end(mp.p_code_end),p_break(mp.p_break),
        imgin(mp.imgin),listin(mp.listin),imgout(mp.imgout),listout(mp.listout),img_stats(mp.img_stats),
        list_stats(mp.list_stats),list_median(mp.list_median),debug_indent(0),result_dim(mp.result_dim),
        break_type(0),constcache_size(0),is_parallelizable(mp.is_parallelizable),is_fill(mp.is_fill),
        need_input_copy(mp.need_input_copy), result(mem._data + (mp.result - mp.mem._data)),calling_function(0) {
#ifdef cimg_use_openmp
        mem[17] = omp_get_thread_num();
#endif
        opcode.assign();
        opcode._is_shared = true;