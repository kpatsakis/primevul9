    //! Display 1d graph in an interactive window \overloading.
    const CImg<T>& display_graph(const char *const title=0,
                                 const unsigned int plot_type=1, const unsigned int vertex_type=1,
                                 const char *const labelx=0, const double xmin=0, const double xmax=0,
                                 const char *const labely=0, const double ymin=0, const double ymax=0,
                                 const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _display_graph(disp,title,plot_type,vertex_type,labelx,xmin,xmax,labely,ymin,ymax,exit_on_anykey);