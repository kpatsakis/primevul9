
      unsigned int scalar2(const mp_func op, const unsigned int arg1, const unsigned int arg2) {
        const unsigned int pos =
          arg1>_cimg_mp_slot_c && _cimg_mp_is_comp(arg1)?arg1:
          arg2>_cimg_mp_slot_c && _cimg_mp_is_comp(arg2)?arg2:scalar();
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2).move_to(code);
        return pos;