WandExport char *DrawGetVectorGraphics(DrawingWand *wand)
{
  char
    value[MagickPathExtent],
    *xml;

  PixelInfo
    pixel;

  register ssize_t
    i;

  XMLTreeInfo
    *child,
    *xml_info;

  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  xml_info=NewXMLTreeTag("drawing-wand");
  if (xml_info == (XMLTreeInfo *) NULL)
    return((char *) NULL);
  (void) SetXMLTreeContent(xml_info," ");
  GetPixelInfo(wand->image,&pixel);
  child=AddChildToXMLTree(xml_info,"clip-path",0);
  if (child != (XMLTreeInfo *) NULL)
    (void) SetXMLTreeContent(child,CurrentContext->clip_mask);
  child=AddChildToXMLTree(xml_info,"clip-units",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickClipPathOptions,(ssize_t) CurrentContext->clip_units),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"decorate",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickDecorateOptions,(ssize_t) CurrentContext->decorate),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"encoding",0);
  if (child != (XMLTreeInfo *) NULL)
    (void) SetXMLTreeContent(child,CurrentContext->encoding);
  child=AddChildToXMLTree(xml_info,"fill",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      if (CurrentContext->fill.alpha != OpaqueAlpha)
        pixel.alpha_trait=CurrentContext->fill.alpha != OpaqueAlpha ?
          BlendPixelTrait : UndefinedPixelTrait;
      pixel=CurrentContext->fill;
      GetColorTuple(&pixel,MagickTrue,value);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"fill-opacity",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        (double) (QuantumScale*CurrentContext->fill.alpha));
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"fill-rule",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickFillRuleOptions,(ssize_t) CurrentContext->fill_rule),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"font",0);
  if (child != (XMLTreeInfo *) NULL)
    (void) SetXMLTreeContent(child,CurrentContext->font);
  child=AddChildToXMLTree(xml_info,"font-family",0);
  if (child != (XMLTreeInfo *) NULL)
    (void) SetXMLTreeContent(child,CurrentContext->family);
  child=AddChildToXMLTree(xml_info,"font-size",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        CurrentContext->pointsize);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"font-stretch",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickStretchOptions,(ssize_t) CurrentContext->stretch),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"font-style",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickStyleOptions,(ssize_t) CurrentContext->style),MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"font-weight",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        CurrentContext->weight);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"gravity",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickGravityOptions,(ssize_t) CurrentContext->gravity),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      if (CurrentContext->stroke.alpha != OpaqueAlpha)
        pixel.alpha_trait=CurrentContext->stroke.alpha != OpaqueAlpha ?
          BlendPixelTrait : UndefinedPixelTrait;
      pixel=CurrentContext->stroke;
      GetColorTuple(&pixel,MagickTrue,value);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-antialias",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%d",
        CurrentContext->stroke_antialias != MagickFalse ? 1 : 0);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-dasharray",0);
  if ((child != (XMLTreeInfo *) NULL) &&
      (CurrentContext->dash_pattern != (double *) NULL))
    {
      char
        *dash_pattern;

      dash_pattern=AcquireString((char *) NULL);
      for (i=0; fabs(CurrentContext->dash_pattern[i]) >= MagickEpsilon; i++)
      {
        if (i != 0)
          (void) ConcatenateString(&dash_pattern,",");
        (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
          CurrentContext->dash_pattern[i]);
        (void) ConcatenateString(&dash_pattern,value);
      }
      (void) SetXMLTreeContent(child,dash_pattern);
      dash_pattern=DestroyString(dash_pattern);
    }
  child=AddChildToXMLTree(xml_info,"stroke-dashoffset",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        CurrentContext->dash_offset);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-linecap",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickLineCapOptions,(ssize_t) CurrentContext->linecap),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-linejoin",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(
        MagickLineJoinOptions,(ssize_t) CurrentContext->linejoin),
        MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-miterlimit",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        CurrentContext->miterlimit);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-opacity",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        (double) (QuantumScale*CurrentContext->stroke.alpha));
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"stroke-width",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        CurrentContext->stroke_width);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"text-align",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CopyMagickString(value,CommandOptionToMnemonic(MagickAlignOptions,
        (ssize_t) CurrentContext->align),MagickPathExtent);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"text-antialias",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%d",
        CurrentContext->text_antialias != MagickFalse ? 1 : 0);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"text-undercolor",0);
  if (child != (XMLTreeInfo *) NULL)
    {
      if (CurrentContext->undercolor.alpha != OpaqueAlpha)
        pixel.alpha_trait=CurrentContext->undercolor.alpha != OpaqueAlpha ?
          BlendPixelTrait : UndefinedPixelTrait;
      pixel=CurrentContext->undercolor;
      GetColorTuple(&pixel,MagickTrue,value);
      (void) SetXMLTreeContent(child,value);
    }
  child=AddChildToXMLTree(xml_info,"vector-graphics",0);
  if (child != (XMLTreeInfo *) NULL)
    (void) SetXMLTreeContent(child,wand->mvg);
  xml=XMLTreeInfoToXML(xml_info);
  xml_info=DestroyXMLTree(xml_info);
  return(xml);
}