
      unsigned int scalar7(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                           const unsigned int arg4, const unsigned int arg5, const unsigned int arg6,
                           const unsigned int arg7) {
        const unsigned int pos =
          arg1>_cimg_mp_slot_c && _cimg_mp_is_comp(arg1)?arg1:
          arg2>_cimg_mp_slot_c && _cimg_mp_is_comp(arg2)?arg2:
          arg3>_cimg_mp_slot_c && _cimg_mp_is_comp(arg3)?arg3:
          arg4>_cimg_mp_slot_c && _cimg_mp_is_comp(arg4)?arg4:
          arg5>_cimg_mp_slot_c && _cimg_mp_is_comp(arg5)?arg5:
          arg6>_cimg_mp_slot_c && _cimg_mp_is_comp(arg6)?arg6:
          arg7>_cimg_mp_slot_c && _cimg_mp_is_comp(arg7)?arg7:scalar();
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2,arg3,arg4,arg5,arg6,arg7).move_to(code);
        return pos;