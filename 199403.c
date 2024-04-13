    template<typename t>
    const CImg<T>& _save_tiff(TIFF *tif, const unsigned int directory, const unsigned int z, const t& pixel_t,
                              const unsigned int compression_type, const float *const voxel_size,
                              const char *const description) const {
      if (is_empty() || !tif || pixel_t) return *this;
      const char *const filename = TIFFFileName(tif);
      uint32 rowsperstrip = (uint32)-1;
      uint16 spp = _spectrum, bpp = sizeof(t)*8, photometric;
      if (spp==3 || spp==4) photometric = PHOTOMETRIC_RGB;
      else photometric = PHOTOMETRIC_MINISBLACK;
      TIFFSetDirectory(tif,directory);
      TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,_width);
      TIFFSetField(tif,TIFFTAG_IMAGELENGTH,_height);
      if (voxel_size) {
        const float vx = voxel_size[0], vy = voxel_size[1], vz = voxel_size[2];
        TIFFSetField(tif,TIFFTAG_RESOLUTIONUNIT,RESUNIT_NONE);
        TIFFSetField(tif,TIFFTAG_XRESOLUTION,1.0f/vx);
        TIFFSetField(tif,TIFFTAG_YRESOLUTION,1.0f/vy);
        CImg<charT> s_description(256);
        cimg_snprintf(s_description,s_description._width,"VX=%g VY=%g VZ=%g spacing=%g",vx,vy,vz,vz);
        TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,s_description.data());
      }
      if (description) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,description);
      TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
      TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,spp);
      if (cimg::type<t>::is_float()) TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,3);
      else if (cimg::type<t>::min()==0) TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,1);
      else TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,2);
      double valm, valM = max_min(valm);
      TIFFSetField(tif,TIFFTAG_SMINSAMPLEVALUE,valm);
      TIFFSetField(tif,TIFFTAG_SMAXSAMPLEVALUE,valM);
      TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bpp);
      TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
      TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,photometric);
      TIFFSetField(tif,TIFFTAG_COMPRESSION,compression_type==2?COMPRESSION_JPEG:
                   compression_type==1?COMPRESSION_LZW:COMPRESSION_NONE);
      rowsperstrip = TIFFDefaultStripSize(tif,rowsperstrip);
      TIFFSetField(tif,TIFFTAG_ROWSPERSTRIP,rowsperstrip);
      TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
      TIFFSetField(tif,TIFFTAG_SOFTWARE,"CImg");

      t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
      if (buf) {
        for (unsigned int row = 0; row<_height; row+=rowsperstrip) {
          uint32 nrow = (row + rowsperstrip>_height?_height - row:rowsperstrip);
          tstrip_t strip = TIFFComputeStrip(tif,row,0);
          tsize_t i = 0;
          for (unsigned int rr = 0; rr<nrow; ++rr)
            for (unsigned int cc = 0; cc<_width; ++cc)
              for (unsigned int vv = 0; vv<spp; ++vv)
                buf[i++] = (t)(*this)(cc,row + rr,z,vv);
          if (TIFFWriteEncodedStrip(tif,strip,buf,i*sizeof(t))<0)
            throw CImgIOException(_cimg_instance
                                  "save_tiff(): Invalid strip writing when saving file '%s'.",
                                  cimg_instance,
                                  filename?filename:"(FILE*)");
        }
        _TIFFfree(buf);
      }
      TIFFWriteDirectory(tif);
      return *this;