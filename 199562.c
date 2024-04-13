    template<typename t>
    void _eval(CImg<t>& output, CImg<T> *const img_output, const char *const expression,
               const double x, const double y, const double z, const double c,
               const CImgList<T> *const list_inputs, CImgList<T> *const list_outputs) const {
      if (!expression || !*expression) { output.assign(1); *output = 0; }
      if (!expression[1]) switch (*expression) { // Single-char optimization.
        case 'w' : output.assign(1); *output = (t)_width; break;
        case 'h' : output.assign(1); *output = (t)_height; break;
        case 'd' : output.assign(1); *output = (t)_depth; break;
        case 's' : output.assign(1); *output = (t)_spectrum; break;
        case 'r' : output.assign(1); *output = (t)_is_shared; break;
        }
      _cimg_math_parser mp(expression + (*expression=='>' || *expression=='<' ||
                                         *expression=='*' || *expression==':'),"eval",
                           *this,img_output,list_inputs,list_outputs,false);
      output.assign(1,std::max(1U,mp.result_dim));
      mp(x,y,z,c,output._data);
      mp.end();