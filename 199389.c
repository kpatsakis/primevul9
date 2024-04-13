    template<typename tf, typename tc>
    const CImg<T>& _save_off(const CImgList<tf>& primitives, const CImgList<tc>& colors,
                             std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_off(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "save_off(): Empty instance, for file '%s'.",
                                    cimg_instance,
                                    filename?filename:"(FILE*)");

      CImgList<T> opacities;
      CImg<charT> error_message(1024);
      if (!is_object3d(primitives,colors,opacities,true,error_message))
        throw CImgInstanceException(_cimg_instance
                                    "save_off(): Invalid specified 3d object, for file '%s' (%s).",
                                    cimg_instance,
                                    filename?filename:"(FILE*)",error_message.data());

      const CImg<tc> default_color(1,3,1,1,200);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      unsigned int supported_primitives = 0;
      cimglist_for(primitives,l) if (primitives[l].size()!=5) ++supported_primitives;
      std::fprintf(nfile,"OFF\n%u %u %u\n",_width,supported_primitives,3*primitives._width);
      cimg_forX(*this,i) std::fprintf(nfile,"%f %f %f\n",
                                      (float)((*this)(i,0)),(float)((*this)(i,1)),(float)((*this)(i,2)));
      cimglist_for(primitives,l) {
        const CImg<tc>& color = l<colors.width()?colors[l]:default_color;
        const unsigned int psiz = primitives[l].size(), csiz = color.size();
        const float r = color[0]/255.0f, g = (csiz>1?color[1]:r)/255.0f, b = (csiz>2?color[2]:g)/255.0f;
        switch (psiz) {
        case 1 : std::fprintf(nfile,"1 %u %f %f %f\n",
                              (unsigned int)primitives(l,0),r,g,b); break;
        case 2 : std::fprintf(nfile,"2 %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,1),r,g,b); break;
        case 3 : std::fprintf(nfile,"3 %u %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,2),
                              (unsigned int)primitives(l,1),r,g,b); break;
        case 4 : std::fprintf(nfile,"4 %u %u %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,3),
                              (unsigned int)primitives(l,2),(unsigned int)primitives(l,1),r,g,b); break;
        case 5 : std::fprintf(nfile,"2 %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,1),r,g,b); break;
        case 6 : {
          const unsigned int xt = (unsigned int)primitives(l,2), yt = (unsigned int)primitives(l,3);
          const float
            rt = color.atXY(xt,yt,0)/255.0f,
            gt = (csiz>1?color.atXY(xt,yt,1):r)/255.0f,
            bt = (csiz>2?color.atXY(xt,yt,2):g)/255.0f;
          std::fprintf(nfile,"2 %u %u %f %f %f\n",
                       (unsigned int)primitives(l,0),(unsigned int)primitives(l,1),rt,gt,bt);
        } break;
        case 9 : {
          const unsigned int xt = (unsigned int)primitives(l,3), yt = (unsigned int)primitives(l,4);
          const float
            rt = color.atXY(xt,yt,0)/255.0f,
            gt = (csiz>1?color.atXY(xt,yt,1):r)/255.0f,
            bt = (csiz>2?color.atXY(xt,yt,2):g)/255.0f;
          std::fprintf(nfile,"3 %u %u %u %f %f %f\n",
                       (unsigned int)primitives(l,0),(unsigned int)primitives(l,2),
                       (unsigned int)primitives(l,1),rt,gt,bt);
        } break;
        case 12 : {
          const unsigned int xt = (unsigned int)primitives(l,4), yt = (unsigned int)primitives(l,5);
          const float
            rt = color.atXY(xt,yt,0)/255.0f,
            gt = (csiz>1?color.atXY(xt,yt,1):r)/255.0f,
            bt = (csiz>2?color.atXY(xt,yt,2):g)/255.0f;
          std::fprintf(nfile,"4 %u %u %u %u %f %f %f\n",
                       (unsigned int)primitives(l,0),(unsigned int)primitives(l,3),
                       (unsigned int)primitives(l,2),(unsigned int)primitives(l,1),rt,gt,bt);
        } break;
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;