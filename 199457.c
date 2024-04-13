
    double _eval(CImg<T> *const img_output, const char *const expression,
                 const double x, const double y, const double z, const double c,
                 const CImgList<T> *const list_inputs, CImgList<T> *const list_outputs) const {
      if (!expression || !*expression) return 0;
      if (!expression[1]) switch (*expression) { // Single-char optimization.
        case 'w' : return (double)_width;
        case 'h' : return (double)_height;
        case 'd' : return (double)_depth;
        case 's' : return (double)_spectrum;
        case 'r' : return (double)_is_shared;
        }
      _cimg_math_parser mp(expression + (*expression=='>' || *expression=='<' ||
                                         *expression=='*' || *expression==':'),"eval",
                           *this,img_output,list_inputs,list_outputs,false);
      const double val = mp(x,y,z,c);
      mp.end();
      return val;