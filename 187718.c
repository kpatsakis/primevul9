WandExport ExceptionInfo *DrawCloneExceptionInfo(const DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->exception == (ExceptionInfo*) NULL)
    return (ExceptionInfo*) NULL;
  return CloneExceptionInfo(wand->exception);
}