      // Insert constant value in memory.
      unsigned int constant(const double val) {

        // Search for built-in constant.
        if (val==(double)(int)val) {
          if (val>=0 && val<=10) return (unsigned int)val;
          if (val<0 && val>=-5) return (unsigned int)(10 - val);
        }
        if (val==0.5) return 16;
        if (cimg::type<double>::is_nan(val)) return _cimg_mp_slot_nan;

        // Search for constant already requested before (in const cache).
        unsigned int ind = ~0U;
        if (constcache_size<1024) {
          if (!constcache_size) {
            constcache_vals.assign(16,1,1,1,0);
            constcache_inds.assign(16,1,1,1,0);
            *constcache_vals = val;
            constcache_size = 1;
            ind = 0;
          } else { // Dichotomic search
            const double val_beg = *constcache_vals, val_end = constcache_vals[constcache_size - 1];
            if (val_beg>=val) ind = 0;
            else if (val_end==val) ind = constcache_size - 1;
            else if (val_end<val) ind = constcache_size;
            else {
              unsigned int i0 = 1, i1 = constcache_size - 2;
              while (i0<=i1) {
                const unsigned int mid = (i0 + i1)/2;
                if (constcache_vals[mid]==val) { i0 = mid; break; }
                else if (constcache_vals[mid]<val) i0 = mid + 1;
                else i1 = mid - 1;
              }
              ind = i0;
            }

            if (ind>=constcache_size || constcache_vals[ind]!=val) {
              ++constcache_size;
              if (constcache_size>constcache_vals._width) {
                constcache_vals.resize(-200,1,1,1,0);
                constcache_inds.resize(-200,1,1,1,0);
              }
              const int l = constcache_size - (int)ind - 1;
              if (l>0) {
                std::memmove(&constcache_vals[ind + 1],&constcache_vals[ind],l*sizeof(double));
                std::memmove(&constcache_inds[ind + 1],&constcache_inds[ind],l*sizeof(unsigned int));
              }
              constcache_vals[ind] = val;
              constcache_inds[ind] = 0;
            }
          }
          if (constcache_inds[ind]) return constcache_inds[ind];
        }

        // Insert new constant in memory if necessary.
        if (mempos>=mem._width) { mem.resize(-200,1,1,1,0); memtype.resize(-200,1,1,1,0); }
        const unsigned int pos = mempos++;
        mem[pos] = val;
        memtype[pos] = 1; // Set constant property
        if (ind!=~0U) constcache_inds[ind] = pos;
        return pos;