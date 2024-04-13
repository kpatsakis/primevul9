png_colorspace_endpoints_match(const png_xy *xy1, const png_xy *xy2, int delta)
{
   /* Allow an error of +/-0.01 (absolute value) on each chromaticity */
   if (PNG_OUT_OF_RANGE(xy1->whitex, xy2->whitex,delta) ||
       PNG_OUT_OF_RANGE(xy1->whitey, xy2->whitey,delta) ||
       PNG_OUT_OF_RANGE(xy1->redx,   xy2->redx,  delta) ||
       PNG_OUT_OF_RANGE(xy1->redy,   xy2->redy,  delta) ||
       PNG_OUT_OF_RANGE(xy1->greenx, xy2->greenx,delta) ||
       PNG_OUT_OF_RANGE(xy1->greeny, xy2->greeny,delta) ||
       PNG_OUT_OF_RANGE(xy1->bluex,  xy2->bluex, delta) ||
       PNG_OUT_OF_RANGE(xy1->bluey,  xy2->bluey, delta))
      return 0;
   return 1;
}