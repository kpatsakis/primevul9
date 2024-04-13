WandExport void DrawSetClipUnits(DrawingWand *wand,
  const ClipPathUnits clip_units)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->clip_units != clip_units))
    {
      CurrentContext->clip_units=clip_units;
      if (clip_units == ObjectBoundingBox)
        {
          AffineMatrix
            affine;

          GetAffineMatrix(&affine);
          affine.sx=CurrentContext->bounds.x2;
          affine.sy=CurrentContext->bounds.y2;
          affine.tx=CurrentContext->bounds.x1;
          affine.ty=CurrentContext->bounds.y1;
          AdjustAffine(wand,&affine);
        }
      (void) MVGPrintf(wand, "clip-units '%s'\n",CommandOptionToMnemonic(
        MagickClipPathOptions,(ssize_t) clip_units));
    }
}