
      static double mp_dowhile(_cimg_math_parser& mp) {
        const ulongT
          mem_body = mp.opcode[1],
          mem_cond = mp.opcode[2];
        const CImg<ulongT>
          *const p_body = ++mp.p_code,
          *const p_cond = p_body + mp.opcode[3],
          *const p_end = p_cond + mp.opcode[4];
        const unsigned int vsiz = (unsigned int)mp.opcode[5];
        if (mp.opcode[6]) { // Set default value for result and condition if necessary
          if (vsiz) CImg<doubleT>(&mp.mem[mem_body] + 1,vsiz,1,1,1,true).fill(cimg::type<double>::nan());
          else mp.mem[mem_body] = cimg::type<double>::nan();
        }
        if (mp.opcode[7]) mp.mem[mem_cond] = 0;

        const unsigned int _break_type = mp.break_type;
        mp.break_type = 0;
        do {
          for (mp.p_code = p_body; mp.p_code<p_cond; ++mp.p_code) { // Evaluate body
            mp.opcode._data = mp.p_code->_data;
            const ulongT target = mp.opcode[1];
            mp.mem[target] = _cimg_mp_defunc(mp);
          }
          if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
          for (mp.p_code = p_cond; mp.p_code<p_end; ++mp.p_code) { // Evaluate condition
            mp.opcode._data = mp.p_code->_data;
            const ulongT target = mp.opcode[1];
            mp.mem[target] = _cimg_mp_defunc(mp);
          }
          if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
        } while (mp.mem[mem_cond]);
        mp.break_type = _break_type;
        mp.p_code = p_end - 1;
        return mp.mem[mem_body];