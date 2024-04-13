                                const bool is_rgb=true) const {
      return _save_yuv(file,0,chroma_subsampling,is_rgb);
    }

    const CImgList<T>& _save_yuv(std::FILE *const file, const char *const filename,
                                 const unsigned int chroma_subsampling,
                                 const bool is_rgb) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_yuv(): Specified filename is (null).",
                                    cimglist_instance);
      if (chroma_subsampling!=420 && chroma_subsampling!=422 && chroma_subsampling!=444)
        throw CImgArgumentException(_cimglist_instance
                                    "save_yuv(): Specified chroma subsampling %u is invalid, for file '%s'.",
                                    cimglist_instance,
                                    chroma_subsampling,filename?filename:"(FILE*)");
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      const unsigned int
        cfx = chroma_subsampling==420 || chroma_subsampling==422?2:1,
        cfy = chroma_subsampling==420?2:1,
        w0 = (*this)[0]._width, h0 = (*this)[0]._height,
        width0 = w0 + (w0%cfx), height0 = h0 + (h0%cfy);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      cimglist_for(*this,l) {
        const CImg<T> &frame = (*this)[l];
        CImg<ucharT> YUV;
        if (sizeof(T)==1 && !is_rgb &&
            frame._width==width0 && frame._height==height0 && frame._depth==1 && frame._spectrum==3)
          YUV.assign((unsigned char*)frame._data,width0,height0,1,3,true);
        else {
          YUV = frame;
          if (YUV._width!=width0 || YUV._height!=height0 || YUV._depth!=1) YUV.resize(width0,height0,1,-100,0);
          if (YUV._spectrum!=3) YUV.resize(-100,-100,1,3,YUV._spectrum==1?1:0);
          if (is_rgb) YUV.RGBtoYCbCr();
        }
        if (chroma_subsampling==444)
          cimg::fwrite(YUV._data,(size_t)YUV._width*YUV._height*3,nfile);
        else {
          cimg::fwrite(YUV._data,(size_t)YUV._width*YUV._height,nfile);
          CImg<ucharT> UV = YUV.get_channels(1,2);
          UV.resize(UV._width/cfx,UV._height/cfy,1,2,2);
          cimg::fwrite(UV._data,(size_t)UV._width*UV._height*2,nfile);
        }