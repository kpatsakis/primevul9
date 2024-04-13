    // (Note: Most of this function has been written by Eric Fausett)
    CImg<T>& _load_png(std::FILE *const file, const char *const filename, unsigned int *const bits_per_pixel) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_png(): Specified filename is (null).",
                                    cimg_instance);

#ifndef cimg_use_png
      cimg::unused(bits_per_pixel);
      if (file)
        throw CImgIOException(_cimg_instance
                              "load_png(): Unable to load data from '(FILE*)' unless libpng is enabled.",
                              cimg_instance);

      else return load_other(filename);
#else
      // Open file and check for PNG validity
#if defined __GNUC__
      const char *volatile nfilename = filename; // Use 'volatile' to avoid (wrong) g++ warning.
      std::FILE *volatile nfile = file?file:cimg::fopen(nfilename,"rb");
#else
      const char *nfilename = filename;
      std::FILE *nfile = file?file:cimg::fopen(nfilename,"rb");
#endif
      unsigned char pngCheck[8] = { 0 };
      cimg::fread(pngCheck,8,(std::FILE*)nfile);
      if (png_sig_cmp(pngCheck,0,8)) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_png(): Invalid PNG file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }

      // Setup PNG structures for read
      png_voidp user_error_ptr = 0;
      png_error_ptr user_error_fn = 0, user_warning_fn = 0;
      png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,user_error_ptr,user_error_fn,user_warning_fn);
      if (!png_ptr) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_png(): Failed to initialize 'png_ptr' structure for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_infop info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,(png_infopp)0,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Failed to initialize 'info_ptr' structure for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_infop end_info = png_create_info_struct(png_ptr);
      if (!end_info) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Failed to initialize 'end_info' structure for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }

      // Error handling callback for png file reading
      if (setjmp(png_jmpbuf(png_ptr))) {
        if (!file) cimg::fclose((std::FILE*)nfile);
        png_destroy_read_struct(&png_ptr, &end_info, (png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Encountered unknown fatal error in libpng for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_init_io(png_ptr, nfile);
      png_set_sig_bytes(png_ptr, 8);

      // Get PNG Header Info up to data block
      png_read_info(png_ptr,info_ptr);
      png_uint_32 W, H;
      int bit_depth, color_type, interlace_type;
      bool is_gray = false;
      png_get_IHDR(png_ptr,info_ptr,&W,&H,&bit_depth,&color_type,&interlace_type,(int*)0,(int*)0);
      if (bits_per_pixel) *bits_per_pixel = (unsigned int)bit_depth;

      // Transforms to unify image data
      if (color_type==PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 8;
      }
      if (color_type==PNG_COLOR_TYPE_GRAY && bit_depth<8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
        is_gray = true;
        bit_depth = 8;
      }
      if (png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        color_type |= PNG_COLOR_MASK_ALPHA;
      }
      if (color_type==PNG_COLOR_TYPE_GRAY || color_type==PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
        color_type |= PNG_COLOR_MASK_COLOR;
        is_gray = true;
      }
      if (color_type==PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr,0xffffU,PNG_FILLER_AFTER);

      png_read_update_info(png_ptr,info_ptr);
      if (bit_depth!=8 && bit_depth!=16) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,&end_info,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Invalid bit depth %u in file '%s'.",
                              cimg_instance,
                              bit_depth,nfilename?nfilename:"(FILE*)");
      }
      const int byte_depth = bit_depth>>3;

      // Allocate Memory for Image Read
      png_bytep *const imgData = new png_bytep[H];
      for (unsigned int row = 0; row<H; ++row) imgData[row] = new png_byte[(size_t)byte_depth*4*W];
      png_read_image(png_ptr,imgData);
      png_read_end(png_ptr,end_info);

      // Read pixel data
      if (color_type!=PNG_COLOR_TYPE_RGB && color_type!=PNG_COLOR_TYPE_RGB_ALPHA) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,&end_info,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Invalid color coding type %u in file '%s'.",
                              cimg_instance,
                              color_type,nfilename?nfilename:"(FILE*)");
      }
      const bool is_alpha = (color_type==PNG_COLOR_TYPE_RGBA);
      try { assign(W,H,1,(is_gray?1:3) + (is_alpha?1:0)); }
      catch (...) { if (!file) cimg::fclose(nfile); throw; }
      T
        *ptr_r = data(0,0,0,0),
        *ptr_g = is_gray?0:data(0,0,0,1),
        *ptr_b = is_gray?0:data(0,0,0,2),
        *ptr_a = !is_alpha?0:data(0,0,0,is_gray?1:3);
      switch (bit_depth) {
      case 8 : {
        cimg_forY(*this,y) {
          const unsigned char *ptrs = (unsigned char*)imgData[y];
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            if (ptr_g) *(ptr_g++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_b) *(ptr_b++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_a) *(ptr_a++) = (T)*(ptrs++); else ++ptrs;
          }
        }
      } break;
      case 16 : {
        cimg_forY(*this,y) {
          const unsigned short *ptrs = (unsigned short*)(imgData[y]);
          if (!cimg::endianness()) cimg::invert_endianness(ptrs,4*_width);
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            if (ptr_g) *(ptr_g++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_b) *(ptr_b++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_a) *(ptr_a++) = (T)*(ptrs++); else ++ptrs;
          }
        }
      } break;
      }
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

      // Deallocate Image Read Memory
      cimg_forY(*this,n) delete[] imgData[n];
      delete[] imgData;
      if (!file) cimg::fclose(nfile);
      return *this;
#endif