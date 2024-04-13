static int TraceMoveTo(FT_Vector *to,DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  (void) FormatLocaleString(path,MaxTextExtent,"M%g,%g",affine.tx+to->x/64.0,
    affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}