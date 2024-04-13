WandExport MagickBooleanType DrawRender(DrawingWand *wand)
{
  MagickBooleanType
    status;

  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  CurrentContext->primitive=wand->mvg;
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"MVG:\n'%s'\n",wand->mvg);
  if (wand->image == (Image *) NULL)
    ThrowDrawException(WandError,"ContainsNoImages",wand->name);
  status=DrawImage(wand->image,CurrentContext,wand->exception);
  CurrentContext->primitive=(char *) NULL;
  return(status);
}