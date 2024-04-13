    //! Autocrop image region, regarding the specified background value.
    CImg<T>& autocrop(const T& value, const char *const axes="czyx") {
      if (is_empty()) return *this;
      for (const char *s = axes; *s; ++s) {
        const char axis = cimg::lowercase(*s);
        const CImg<intT> coords = _autocrop(value,axis);
        if (coords[0]==-1 && coords[1]==-1) return assign(); // Image has only 'value' pixels.
        else switch (axis) {
        case 'x' : {
	  const int x0 = coords[0], x1 = coords[1];
	  if (x0>=0 && x1>=0) crop(x0,x1);
	} break;
        case 'y' : {
	  const int y0 = coords[0], y1 = coords[1];
	  if (y0>=0 && y1>=0) crop(0,y0,_width - 1,y1);
	} break;
        case 'z' : {
	  const int z0 = coords[0], z1 = coords[1];
	  if (z0>=0 && z1>=0) crop(0,0,z0,_width - 1,_height - 1,z1);
	} break;
        default : {
	  const int c0 = coords[0], c1 = coords[1];
	  if (c0>=0 && c1>=0) crop(0,0,0,c0,_width - 1,_height - 1,_depth - 1,c1);
	}
        }
      }
      return *this;