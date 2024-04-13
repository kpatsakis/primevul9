      // Constructors.
      _cimg_math_parser(const char *const expression, const char *const funcname=0,
                        const CImg<T>& img_input=CImg<T>::const_empty(), CImg<T> *const img_output=0,
                        const CImgList<T> *const list_inputs=0, CImgList<T> *const list_outputs=0,
                        const bool _is_fill=false):
        code(_code),p_break((CImg<ulongT>*)0 - 2),
        imgin(img_input),listin(list_inputs?*list_inputs:CImgList<T>::const_empty()),
        imgout(img_output?*img_output:CImg<T>::empty()),listout(list_outputs?*list_outputs:CImgList<T>::empty()),
        img_stats(_img_stats),list_stats(_list_stats),list_median(_list_median),user_macro(0),
        mem_img_median(~0U),debug_indent(0),result_dim(0),break_type(0),constcache_size(0),
        is_parallelizable(true),is_fill(_is_fill),need_input_copy(false),
        calling_function(funcname?funcname:"cimg_math_parser") {
        if (!expression || !*expression)
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Empty expression.",
                                      pixel_type(),_cimg_mp_calling_function);
        const char *_expression = expression;
        while (*_expression && ((signed char)*_expression<=' ' || *_expression==';')) ++_expression;
        CImg<charT>::string(_expression).move_to(expr);
        char *ps = &expr.back() - 1;
        while (ps>expr._data && ((signed char)*ps<=' ' || *ps==';')) --ps;
        *(++ps) = 0; expr._width = (unsigned int)(ps - expr._data + 1);

        // Ease the retrieval of previous non-space characters afterwards.
        pexpr.assign(expr._width);
        char c, *pe = pexpr._data;
        for (ps = expr._data, c = ' '; *ps; ++ps) {
          if ((signed char)*ps>' ') c = *ps; else *ps = ' ';
          *(pe++) = c;
        }
        *pe = 0;
        level = get_level(expr);

        // Init constant values.
#define _cimg_mp_interpolation (reserved_label[29]!=~0U?reserved_label[29]:0)
#define _cimg_mp_boundary (reserved_label[30]!=~0U?reserved_label[30]:0)
#define _cimg_mp_slot_nan 29
#define _cimg_mp_slot_x 30
#define _cimg_mp_slot_y 31
#define _cimg_mp_slot_z 32
#define _cimg_mp_slot_c 33

        mem.assign(96);
        for (unsigned int i = 0; i<=10; ++i) mem[i] = (double)i; // mem[0-10] = 0...10
        for (unsigned int i = 1; i<=5; ++i) mem[i + 10] = -(double)i; // mem[11-15] = -1...-5
        mem[16] = 0.5;
        mem[17] = 0; // thread_id
        mem[18] = (double)imgin._width; // w
        mem[19] = (double)imgin._height; // h
        mem[20] = (double)imgin._depth; // d
        mem[21] = (double)imgin._spectrum; // s
        mem[22] = (double)imgin._is_shared; // r
        mem[23] = (double)imgin._width*imgin._height; // wh
        mem[24] = (double)imgin._width*imgin._height*imgin._depth; // whd
        mem[25] = (double)imgin._width*imgin._height*imgin._depth*imgin._spectrum; // whds
        mem[26] = (double)listin._width; // l
        mem[27] = std::exp(1.0); // e
        mem[28] = cimg::PI; // pi
        mem[_cimg_mp_slot_nan] = cimg::type<double>::nan(); // nan

        // Set value property :
        // { -2 = other | -1 = variable | 0 = computation value |
        //    1 = compile-time constant | N>1 = constant ptr to vector[N-1] }.
        memtype.assign(mem._width,1,1,1,0);
        for (unsigned int i = 0; i<_cimg_mp_slot_x; ++i) memtype[i] = 1;
        memtype[17] = 0;
        memtype[_cimg_mp_slot_x] = memtype[_cimg_mp_slot_y] = memtype[_cimg_mp_slot_z] = memtype[_cimg_mp_slot_c] = -2;
        mempos = _cimg_mp_slot_c + 1;
        variable_pos.assign(8);

        reserved_label.assign(128,1,1,1,~0U);
        // reserved_label[4-28] are used to store these two-char variables:
        // [0] = wh, [1] = whd, [2] = whds, [3] = pi, [4] = im, [5] = iM, [6] = ia, [7] = iv,
        // [8] = is, [9] = ip, [10] = ic, [11] = xm, [12] = ym, [13] = zm, [14] = cm, [15] = xM,
        // [16] = yM, [17] = zM, [18]=cM, [19]=i0...[28]=i9, [29] = interpolation, [30] = boundary

        // Compile expression into a serie of opcodes.
        s_op = ""; ss_op = expr._data;
        const unsigned int ind_result = compile(expr._data,expr._data + expr._width - 1,0,0,false);
        if (!_cimg_mp_is_constant(ind_result)) {
          if (_cimg_mp_is_vector(ind_result))
            CImg<doubleT>(&mem[ind_result] + 1,_cimg_mp_size(ind_result),1,1,1,true).
              fill(cimg::type<double>::nan());
          else mem[ind_result] = cimg::type<double>::nan();
        }

        // Free resources used for compiling expression and prepare evaluation.
        result_dim = _cimg_mp_size(ind_result);
        if (mem._width>=256 && mem._width - mempos>=mem._width/2) mem.resize(mempos,1,1,1,-1);
        result = mem._data + ind_result;
        memtype.assign();
        constcache_vals.assign();
        constcache_inds.assign();
        level.assign();
        variable_pos.assign();
        reserved_label.assign();
        expr.assign();
        pexpr.assign();
        opcode.assign();
        opcode._is_shared = true;

        // Execute init() bloc if any specified.
        if (code_init) {
          mem[_cimg_mp_slot_x] = mem[_cimg_mp_slot_y] = mem[_cimg_mp_slot_z] = mem[_cimg_mp_slot_c] = 0;
          p_code_end = code_init.end();
          for (p_code = code_init; p_code<p_code_end; ++p_code) {
            opcode._data = p_code->_data;
            const ulongT target = opcode[1];
            mem[target] = _cimg_mp_defunc(*this);
          }
        }
        p_code_end = code.end();