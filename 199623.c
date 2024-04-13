    **/
    inline void info() {
      std::fprintf(cimg::output(),"\n %s%sCImg Library %u.%u.%u%s, compiled %s ( %s ) with the following flags:\n\n",
                   cimg::t_red,cimg::t_bold,cimg_version/100,(cimg_version/10)%10,cimg_version%10,
                   cimg::t_normal,cimg_date,cimg_time);

      std::fprintf(cimg::output(),"  > Operating System:       %s%-13s%s %s('cimg_OS'=%d)%s\n",
                   cimg::t_bold,
                   cimg_OS==1?"Unix":(cimg_OS==2?"Windows":"Unknow"),
                   cimg::t_normal,cimg::t_green,
                   cimg_OS,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > CPU endianness:         %s%s Endian%s\n",
                   cimg::t_bold,
                   cimg::endianness()?"Big":"Little",
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Verbosity mode:         %s%-13s%s %s('cimg_verbosity'=%d)%s\n",
                   cimg::t_bold,
                   cimg_verbosity==0?"Quiet":
                   cimg_verbosity==1?"Console":
                   cimg_verbosity==2?"Dialog":
                   cimg_verbosity==3?"Console+Warnings":"Dialog+Warnings",
                   cimg::t_normal,cimg::t_green,
                   cimg_verbosity,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Stricts warnings:       %s%-13s%s %s('cimg_strict_warnings' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_strict_warnings
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Support for C++11:      %s%-13s%s %s('cimg_use_cpp11'=%d)%s\n",
                   cimg::t_bold,
                   cimg_use_cpp11?"Yes":"No",
                   cimg::t_normal,cimg::t_green,
                   (int)cimg_use_cpp11,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using VT100 messages:   %s%-13s%s %s('cimg_use_vt100' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_vt100
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Display type:           %s%-13s%s %s('cimg_display'=%d)%s\n",
                   cimg::t_bold,
                   cimg_display==0?"No display":cimg_display==1?"X11":cimg_display==2?"Windows GDI":"Unknown",
                   cimg::t_normal,cimg::t_green,
                   (int)cimg_display,
                   cimg::t_normal);

#if cimg_display==1
      std::fprintf(cimg::output(),"  > Using XShm for X11:     %s%-13s%s %s('cimg_use_xshm' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_xshm
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using XRand for X11:    %s%-13s%s %s('cimg_use_xrandr' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_xrandr
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);
#endif
      std::fprintf(cimg::output(),"  > Using OpenMP:           %s%-13s%s %s('cimg_use_openmp' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_openmp
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);
      std::fprintf(cimg::output(),"  > Using PNG library:      %s%-13s%s %s('cimg_use_png' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_png
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);
      std::fprintf(cimg::output(),"  > Using JPEG library:     %s%-13s%s %s('cimg_use_jpeg' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_jpeg
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using TIFF library:     %s%-13s%s %s('cimg_use_tiff' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_tiff
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using Magick++ library: %s%-13s%s %s('cimg_use_magick' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_magick
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using FFTW3 library:    %s%-13s%s %s('cimg_use_fftw3' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_fftw3
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using LAPACK library:   %s%-13s%s %s('cimg_use_lapack' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_lapack
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      char *const tmp = new char[1024];
      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::imagemagick_path());
      std::fprintf(cimg::output(),"  > Path of ImageMagick:    %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::graphicsmagick_path());
      std::fprintf(cimg::output(),"  > Path of GraphicsMagick: %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::medcon_path());
      std::fprintf(cimg::output(),"  > Path of 'medcon':       %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::temporary_path());
      std::fprintf(cimg::output(),"  > Temporary path:         %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"\n");
      delete[] tmp;