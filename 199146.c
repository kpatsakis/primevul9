     \param file Input file (can be \c 0 if \c filename is set).
     \param filename Filename, as a C-string (can be \c 0 if \c file is set).
     \return C-string containing the guessed file format, or \c 0 if nothing has been guessed.
  **/
  inline const char *ftype(std::FILE *const file, const char *const filename) {
    if (!file && !filename)
      throw CImgArgumentException("cimg::ftype(): Specified filename is (null).");
    static const char
      *const _pnm = "pnm",
      *const _pfm = "pfm",
      *const _bmp = "bmp",
      *const _gif = "gif",
      *const _jpg = "jpg",
      *const _off = "off",
      *const _pan = "pan",
      *const _png = "png",
      *const _tif = "tif",
      *const _inr = "inr",
      *const _dcm = "dcm";
    const char *f_type = 0;
    CImg<char> header;
    const unsigned int omode = cimg::exception_mode();
    cimg::exception_mode(0);
    try {
      header._load_raw(file,filename,512,1,1,1,false,false,0);
      const unsigned char *const uheader = (unsigned char*)header._data;
      if (!std::strncmp(header,"OFF\n",4)) f_type = _off; // OFF.
      else if (!std::strncmp(header,"#INRIMAGE",9)) f_type = _inr; // INRIMAGE.
      else if (!std::strncmp(header,"PANDORE",7)) f_type = _pan; // PANDORE.
      else if (!std::strncmp(header.data() + 128,"DICM",4)) f_type = _dcm; // DICOM.
      else if (uheader[0]==0xFF && uheader[1]==0xD8 && uheader[2]==0xFF) f_type = _jpg;  // JPEG.
      else if (header[0]=='B' && header[1]=='M') f_type = _bmp;  // BMP.
      else if (header[0]=='G' && header[1]=='I' && header[2]=='F' && header[3]=='8' && header[5]=='a' && // GIF.
               (header[4]=='7' || header[4]=='9')) f_type = _gif;
      else if (uheader[0]==0x89 && uheader[1]==0x50 && uheader[2]==0x4E && uheader[3]==0x47 &&  // PNG.
               uheader[4]==0x0D && uheader[5]==0x0A && uheader[6]==0x1A && uheader[7]==0x0A) f_type = _png;
      else if ((uheader[0]==0x49 && uheader[1]==0x49) || (uheader[0]==0x4D && uheader[1]==0x4D)) f_type = _tif; // TIFF.
      else { // PNM or PFM.
        CImgList<char> _header = header.get_split(CImg<char>::vector('\n'),0,false);
        cimglist_for(_header,l) {
          if (_header(l,0)=='#') continue;
          if (_header[l]._height==2 && _header(l,0)=='P') {
            const char c = _header(l,1);
            if (c=='f' || c=='F') { f_type = _pfm; break; }
            if (c>='1' && c<='9') { f_type = _pnm; break; }
          }
          f_type = 0; break;
        }
      }