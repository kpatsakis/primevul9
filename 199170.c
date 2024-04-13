
      static double mp_crop(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const int x = (int)_mp_arg(3), y = (int)_mp_arg(4), z = (int)_mp_arg(5), c = (int)_mp_arg(6);
        const unsigned int
          dx = (unsigned int)mp.opcode[7],
          dy = (unsigned int)mp.opcode[8],
          dz = (unsigned int)mp.opcode[9],
          dc = (unsigned int)mp.opcode[10];
        const bool boundary_conditions = (bool)_mp_arg(11);
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        const CImg<T> &img = ind==~0U?mp.imgin:mp.listin[ind];
        if (!img) std::memset(ptrd,0,dx*dy*dz*dc*sizeof(double));
        else CImg<doubleT>(ptrd,dx,dy,dz,dc,true) = img.get_crop(x,y,z,c,
                                                                 x + dx - 1,y + dy - 1,
                                                                 z + dz - 1,c + dc - 1,
                                                                 boundary_conditions);
        return cimg::type<double>::nan();