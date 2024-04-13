WandExport void DrawAffine(DrawingWand *wand,const AffineMatrix *affine)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(affine != (const AffineMatrix *) NULL);
  AdjustAffine(wand,affine);
  (void) MVGPrintf(wand,"affine %.20g %.20g %.20g %.20g %.20g %.20g\n",
    affine->sx,affine->rx,affine->ry,affine->sy,affine->tx,affine->ty);
}