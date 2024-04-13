WandExport void DrawSetGravity(DrawingWand *wand,const GravityType gravity)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->gravity != gravity) || (gravity != ForgetGravity))
    {
      CurrentContext->gravity=gravity;
      (void) MVGPrintf(wand,"gravity '%s'\n",CommandOptionToMnemonic(
        MagickGravityOptions,(ssize_t) gravity));
    }
}