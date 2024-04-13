static int TraceCubicBezier(FT_Vector *p,FT_Vector *q,FT_Vector *to,
  DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  (void) FormatLocaleString(path,MaxTextExtent,"C%g,%g %g,%g %g,%g",affine.tx+
    p->x/64.0,affine.ty-p->y/64.0,affine.tx+q->x/64.0,affine.ty-q->y/64.0,
    affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}