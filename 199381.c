
      _cimg_math_parser():
        code(_code),p_code_end(0),p_break((CImg<ulongT>*)0 - 2),
        imgin(CImg<T>::const_empty()),listin(CImgList<T>::const_empty()),
        imgout(CImg<T>::empty()),listout(CImgList<T>::empty()),
        img_stats(_img_stats),list_stats(_list_stats),list_median(_list_median),debug_indent(0),
        result_dim(0),break_type(0),constcache_size(0),is_parallelizable(true),is_fill(false),need_input_copy(false),
        calling_function(0) {
        mem.assign(1 + _cimg_mp_slot_c,1,1,1,0); // Allow to skip 'is_empty?' test in operator()()
        result = mem._data;