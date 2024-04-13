    //! Simple interface to select a shape from an image \newinstance.
    CImg<intT> get_select(const char *const title,
    			  const unsigned int feature_type=2, unsigned int *const XYZ=0,
                          const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _select(disp,title,feature_type,XYZ,0,0,0,exit_on_anykey,true,false);