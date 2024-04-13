WandExport void DrawSetClipRule(DrawingWand *wand,const FillRule fill_rule)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule=fill_rule;
      (void) MVGPrintf(wand, "clip-rule '%s'\n",CommandOptionToMnemonic(
        MagickFillRuleOptions,(ssize_t) fill_rule));
    }
}