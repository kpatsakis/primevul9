
    void _set_colormap(Colormap& _colormap, const unsigned int dim) {
      XColor *const colormap = new XColor[256];
      switch (dim) {
      case 1 : { // colormap for greyscale images
        for (unsigned int index = 0; index<256; ++index) {
          colormap[index].pixel = index;
          colormap[index].red = colormap[index].green = colormap[index].blue = (unsigned short)(index<<8);
          colormap[index].flags = DoRed | DoGreen | DoBlue;
        }
      } break;
      case 2 : { // colormap for RG images
        for (unsigned int index = 0, r = 8; r<256; r+=16)
          for (unsigned int g = 8; g<256; g+=16) {
            colormap[index].pixel = index;
            colormap[index].red = colormap[index].blue = (unsigned short)(r<<8);
            colormap[index].green = (unsigned short)(g<<8);
            colormap[index++].flags = DoRed | DoGreen | DoBlue;
          }
      } break;
      default : { // colormap for RGB images
        for (unsigned int index = 0, r = 16; r<256; r+=32)
          for (unsigned int g = 16; g<256; g+=32)
            for (unsigned int b = 32; b<256; b+=64) {
              colormap[index].pixel = index;
              colormap[index].red = (unsigned short)(r<<8);
              colormap[index].green = (unsigned short)(g<<8);
              colormap[index].blue = (unsigned short)(b<<8);
              colormap[index++].flags = DoRed | DoGreen | DoBlue;
            }
      }
      }
      XStoreColors(cimg::X11_attr().display,_colormap,colormap,256);
      delete[] colormap;