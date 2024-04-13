    **/
    CImg<T>& select(CImgDisplay &disp,
		    const unsigned int feature_type=2, unsigned int *const XYZ=0,
                    const bool exit_on_anykey=false) {
      return get_select(disp,feature_type,XYZ,exit_on_anykey).move_to(*this);