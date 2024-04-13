WandExport MagickBooleanType DrawSetVectorGraphics(DrawingWand *wand,
  const char *xml)
{
  const char
    *value;

  XMLTreeInfo
    *child,
    *xml_info;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  CurrentContext=DestroyDrawInfo(CurrentContext);
  CurrentContext=CloneDrawInfo((ImageInfo *) NULL,(DrawInfo *) NULL);
  if (xml == (const char *) NULL)
    return(MagickFalse);
  xml_info=NewXMLTree(xml,wand->exception);
  if (xml_info == (XMLTreeInfo *) NULL)
    return(MagickFalse);
  child=GetXMLTreeChild(xml_info,"clip-path");
  if (child != (XMLTreeInfo *) NULL)
    (void) CloneString(&CurrentContext->clip_mask,GetXMLTreeContent(child));
  child=GetXMLTreeChild(xml_info,"clip-units");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->clip_units=(ClipPathUnits) ParseCommandOption(
          MagickClipPathOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"decorate");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->decorate=(DecorationType) ParseCommandOption(
          MagickDecorateOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"encoding");
  if (child != (XMLTreeInfo *) NULL)
    (void) CloneString(&CurrentContext->encoding,GetXMLTreeContent(child));
  child=GetXMLTreeChild(xml_info,"fill");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        (void) QueryColorCompliance(value,AllCompliance,&CurrentContext->fill,
          wand->exception);
    }
  child=GetXMLTreeChild(xml_info,"fill-opacity");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->fill.alpha=(double) ClampToQuantum(QuantumRange*
          (1.0-StringToDouble(value,(char **) NULL)));
    }
  child=GetXMLTreeChild(xml_info,"fill-rule");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->fill_rule=(FillRule) ParseCommandOption(
          MagickFillRuleOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"font");
  if (child != (XMLTreeInfo *) NULL)
    (void) CloneString(&CurrentContext->font,GetXMLTreeContent(child));
  child=GetXMLTreeChild(xml_info,"font-family");
  if (child != (XMLTreeInfo *) NULL)
    (void) CloneString(&CurrentContext->family,GetXMLTreeContent(child));
  child=GetXMLTreeChild(xml_info,"font-size");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->pointsize=StringToDouble(value,(char **) NULL);
    }
  child=GetXMLTreeChild(xml_info,"font-stretch");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->stretch=(StretchType) ParseCommandOption(
          MagickStretchOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"font-style");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->style=(StyleType) ParseCommandOption(MagickStyleOptions,
          MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"font-weight");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        {
          ssize_t
            weight;

          weight=ParseCommandOption(MagickWeightOptions,MagickFalse,value);
          if (weight == -1)
            weight=StringToUnsignedLong(value);
          CurrentContext->weight=weight;
        }
    }
  child=GetXMLTreeChild(xml_info,"gravity");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->gravity=(GravityType) ParseCommandOption(
          MagickGravityOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"stroke");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        (void) QueryColorCompliance(value,AllCompliance,&CurrentContext->stroke,
          wand->exception);
    }
  child=GetXMLTreeChild(xml_info,"stroke-antialias");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->stroke_antialias=StringToLong(value) != 0 ? MagickTrue :
          MagickFalse;
    }
  child=GetXMLTreeChild(xml_info,"stroke-dasharray");
  if (child != (XMLTreeInfo *) NULL)
    {
      char
        token[MagickPathExtent];

      const char
        *q;

      register ssize_t
        x;

      ssize_t
        j;

      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        {
          if (CurrentContext->dash_pattern != (double *) NULL)
            CurrentContext->dash_pattern=(double *) RelinquishMagickMemory(
              CurrentContext->dash_pattern);
          q=(char *) value;
          if (IsPoint(q) != MagickFalse)
            {
              const char
                *p;

              p=q;
              GetNextToken(p,&p,MagickPathExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MagickPathExtent,token);
              for (x=0; IsPoint(token) != MagickFalse; x++)
              {
                GetNextToken(p,&p,MagickPathExtent,token);
                if (*token == ',')
                  GetNextToken(p,&p,MagickPathExtent,token);
              }
              CurrentContext->dash_pattern=(double *) AcquireQuantumMemory(
                (size_t) (2UL*x)+1UL,sizeof(*CurrentContext->dash_pattern));
              if (CurrentContext->dash_pattern == (double *) NULL)
                ThrowWandFatalException(ResourceLimitFatalError,
                  "MemoryAllocationFailed",wand->name);
              for (j=0; j < x; j++)
              {
                GetNextToken(q,&q,MagickPathExtent,token);
                if (*token == ',')
                  GetNextToken(q,&q,MagickPathExtent,token);
                CurrentContext->dash_pattern[j]=StringToDouble(token,
                  (char **) NULL);
              }
              if ((x & 0x01) != 0)
                for ( ; j < (2*x); j++)
                  CurrentContext->dash_pattern[j]=
                    CurrentContext->dash_pattern[j-x];
              CurrentContext->dash_pattern[j]=0.0;
            }
        }
    }
  child=GetXMLTreeChild(xml_info,"stroke-dashoffset");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->dash_offset=StringToDouble(value,(char **) NULL);
    }
  child=GetXMLTreeChild(xml_info,"stroke-linecap");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->linecap=(LineCap) ParseCommandOption(
          MagickLineCapOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"stroke-linejoin");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->linejoin=(LineJoin) ParseCommandOption(
          MagickLineJoinOptions,MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"stroke-miterlimit");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->miterlimit=StringToUnsignedLong(value);
    }
  child=GetXMLTreeChild(xml_info,"stroke-opacity");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->stroke.alpha=(double) ClampToQuantum(QuantumRange*
          (1.0-StringToDouble(value,(char **) NULL)));
    }
  child=GetXMLTreeChild(xml_info,"stroke-width");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->stroke_width=StringToDouble(value,(char **) NULL);
    }
  child=GetXMLTreeChild(xml_info,"text-align");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->align=(AlignType) ParseCommandOption(MagickAlignOptions,
          MagickFalse,value);
    }
  child=GetXMLTreeChild(xml_info,"text-antialias");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        CurrentContext->text_antialias=StringToLong(value) != 0 ? MagickTrue :
          MagickFalse;
    }
  child=GetXMLTreeChild(xml_info,"text-undercolor");
  if (child != (XMLTreeInfo *) NULL)
    {
      value=GetXMLTreeContent(child);
      if (value != (const char *) NULL)
        (void) QueryColorCompliance(value,AllCompliance,
          &CurrentContext->undercolor,wand->exception);
    }
  child=GetXMLTreeChild(xml_info,"vector-graphics");
  if (child != (XMLTreeInfo *) NULL)
    {
      (void) CloneString(&wand->mvg,GetXMLTreeContent(child));
      wand->mvg_length=strlen(wand->mvg);
      wand->mvg_alloc=wand->mvg_length+1;
    }
  xml_info=DestroyXMLTree(xml_info);
  return(MagickTrue);
}