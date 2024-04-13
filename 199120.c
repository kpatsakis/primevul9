    **/
    CImg<T>& fill(const char *const expression, const bool repeat_values, const bool allow_formula=true,
                  const CImgList<T> *const list_inputs=0, CImgList<T> *const list_outputs=0) {
      return _fill(expression,repeat_values,allow_formula,list_inputs,list_outputs,"fill",0);