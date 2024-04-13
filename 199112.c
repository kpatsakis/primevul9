
      static float* _mp_memcopy_float(_cimg_math_parser& mp, const ulongT *const p_ref,
                                      const longT siz, const long inc) {
        const unsigned ind = (unsigned int)p_ref[1];
        const CImg<T> &img = ind==~0U?mp.imgin:mp.listin[cimg::mod((int)mp.mem[ind],mp.listin.width())];
        const bool is_relative = (bool)p_ref[2];
        int ox, oy, oz, oc;
        longT off = 0;
        if (is_relative) {
          ox = (int)mp.mem[_cimg_mp_slot_x];
          oy = (int)mp.mem[_cimg_mp_slot_y];
          oz = (int)mp.mem[_cimg_mp_slot_z];
          oc = (int)mp.mem[_cimg_mp_slot_c];
          off = img.offset(ox,oy,oz,oc);
        }
        if ((*p_ref)%2) {
          const int
            x = (int)mp.mem[p_ref[3]],
            y = (int)mp.mem[p_ref[4]],
            z = (int)mp.mem[p_ref[5]],
            c = *p_ref==5?0:(int)mp.mem[p_ref[6]];
          off+=img.offset(x,y,z,c);
        } else off+=(longT)mp.mem[p_ref[3]];
        const longT eoff = off + (siz - 1)*inc;
        if (off<0 || eoff>=(longT)img.size())
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'copy()': "
                                      "Out-of-bounds image pointer "
                                      "(length: %ld, increment: %ld, offset start: %ld, "
                                      "offset end: %ld, offset max: %lu).",
                                      mp.imgin.pixel_type(),siz,inc,off,eoff,img.size() - 1);
        return (float*)&img[off];