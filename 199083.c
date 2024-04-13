
      static double mp_find_seq(_cimg_math_parser& mp) {
        const bool is_forward = (bool)_mp_arg(6);
        const ulongT
          siz1 = (ulongT)mp.opcode[3],
          siz2 = (ulongT)mp.opcode[5];
        longT ind = (longT)(mp.opcode[7]!=_cimg_mp_slot_nan?_mp_arg(7):is_forward?0:siz1 - 1);
        if (ind<0 || ind>=(longT)siz1) return -1.;
        const double
          *const ptr1b = &_mp_arg(2) + 1,
          *const ptr1e = ptr1b + siz1,
          *const ptr2b = &_mp_arg(4) + 1,
          *const ptr2e = ptr2b + siz2,
          *ptr1 = ptr1b + ind,
          *p1 = 0,
          *p2 = 0;

        // Forward search.
        if (is_forward) {
          do {
            while (ptr1<ptr1e && *ptr1!=*ptr2b) ++ptr1;
            p1 = ptr1 + 1;
            p2 = ptr2b + 1;
            while (p1<ptr1e && p2<ptr2e && *p1==*p2) { ++p1; ++p2; }
          } while (p2<ptr2e && ++ptr1<ptr1e);
          return p2<ptr2e?-1.0:(double)(ptr1 - ptr1b);
        }

        // Backward search.
        do {
          while (ptr1>=ptr1b && *ptr1!=*ptr2b) --ptr1;
          p1 = ptr1 + 1;
          p2 = ptr2b + 1;
          while (p1<ptr1e && p2<ptr2e && *p1==*p2) { ++p1; ++p2; }
        } while (p2<ptr2e && --ptr1>=ptr1b);
        return p2<ptr2e?-1.0:(double)(ptr1 - ptr1b);