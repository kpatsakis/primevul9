static void AdjustAffine(DrawingWand *wand,const AffineMatrix *affine)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((affine->sx != 1.0) || (affine->rx != 0.0) || (affine->ry != 0.0) ||
      (affine->sy != 1.0) || (affine->tx != 0.0) || (affine->ty != 0.0))
    {
      AffineMatrix
        current;

      current=CurrentContext->affine;
      CurrentContext->affine.sx=affine->sx*current.sx+affine->ry*current.rx;
      CurrentContext->affine.rx=affine->rx*current.sx+affine->sy*current.rx;
      CurrentContext->affine.ry=affine->sx*current.ry+affine->ry*current.sy;
      CurrentContext->affine.sy=affine->rx*current.ry+affine->sy*current.sy;
      CurrentContext->affine.tx=affine->sx*current.tx+affine->ry*current.ty+
        affine->tx;
      CurrentContext->affine.ty=affine->rx*current.tx+affine->sy*current.ty+
        affine->ty;
    }
}