
      unsigned int scalar1(const mp_func op, const unsigned int arg1) {
        const unsigned int pos =
          arg1>_cimg_mp_slot_c && _cimg_mp_is_comp(arg1) && op!=mp_copy?arg1:scalar();
        CImg<ulongT>::vector((ulongT)op,pos,arg1).move_to(code);
        return pos;