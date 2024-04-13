      // Evaluation procedure for the end() blocks.
      void end() {
        if (code_end.is_empty()) return;
        if (imgin) {
          mem[_cimg_mp_slot_x] = imgin._width - 1.0;
          mem[_cimg_mp_slot_y] = imgin._height - 1.0f;
          mem[_cimg_mp_slot_z] = imgin._depth - 1.0f;
          mem[_cimg_mp_slot_c] = imgin._spectrum - 1.0f;
        } else mem[_cimg_mp_slot_x] = mem[_cimg_mp_slot_y] = mem[_cimg_mp_slot_z] = mem[_cimg_mp_slot_c] = 0;
        p_code_end = code_end.end();
        for (p_code = code_end; p_code<p_code_end; ++p_code) {
          opcode._data = p_code->_data;
          const ulongT target = opcode[1];
          mem[target] = _cimg_mp_defunc(*this);
        }