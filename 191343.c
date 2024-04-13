static MagickBooleanType RenderFreetype(Image *image,const DrawInfo *draw_info,
  const char *encoding,const PointInfo *offset,TypeMetric *metrics)
{
#if !defined(FT_OPEN_PATHNAME)
#define FT_OPEN_PATHNAME  ft_open_pathname
#endif

  typedef struct _GlyphInfo
  {
    FT_UInt
      id;

    FT_Vector
      origin;

    FT_Glyph
      image;
  } GlyphInfo;

  const char
    *value;

  DrawInfo
    *annotate_info;

  ExceptionInfo
    *exception;

  FT_BBox
    bounds;

  FT_BitmapGlyph
    bitmap;

  FT_Encoding
    encoding_type;

  FT_Error
    ft_status;

  FT_Face
    face;

  FT_Int32
    flags;

  FT_Library
    library;

  FT_Matrix
    affine;

  FT_Open_Args
    args;

  FT_Vector
    origin;

  GlyphInfo
    glyph,
    last_glyph;

  GraphemeInfo
    *grapheme;

  MagickBooleanType
    status;

  PointInfo
    point,
    resolution;

  register char
    *p;

  register ssize_t
    i;

  size_t
    length;

  ssize_t
    code,
    y;

  static FT_Outline_Funcs
    OutlineMethods =
    {
      (FT_Outline_MoveTo_Func) TraceMoveTo,
      (FT_Outline_LineTo_Func) TraceLineTo,
      (FT_Outline_ConicTo_Func) TraceQuadraticBezier,
      (FT_Outline_CubicTo_Func) TraceCubicBezier,
      0, 0
    };

  unsigned char
    *utf8;

  /*
    Initialize Truetype library.
  */
  exception=(&image->exception);
  ft_status=FT_Init_FreeType(&library);
  if (ft_status != 0)
    ThrowBinaryException(TypeError,"UnableToInitializeFreetypeLibrary",
      image->filename);
  args.flags=FT_OPEN_PATHNAME;
  if (draw_info->font == (char *) NULL)
    args.pathname=ConstantString("helvetica");
  else
    if (*draw_info->font != '@')
      args.pathname=ConstantString(draw_info->font);
    else
      args.pathname=ConstantString(draw_info->font+1);
  face=(FT_Face) NULL;
  ft_status=FT_Open_Face(library,&args,(long) draw_info->face,&face);
  if (ft_status != 0)
    {
      (void) FT_Done_FreeType(library);
      (void) ThrowMagickException(exception,GetMagickModule(),TypeError,
        "UnableToReadFont","`%s'",args.pathname);
      args.pathname=DestroyString(args.pathname);
      return(MagickFalse);
    }
  args.pathname=DestroyString(args.pathname);
  if ((draw_info->metrics != (char *) NULL) &&
      (IsPathAccessible(draw_info->metrics) != MagickFalse))
    (void) FT_Attach_File(face,draw_info->metrics);
  encoding_type=FT_ENCODING_UNICODE;
  ft_status=FT_Select_Charmap(face,encoding_type);
  if ((ft_status != 0) && (face->num_charmaps != 0))
    ft_status=FT_Set_Charmap(face,face->charmaps[0]);
  if (encoding != (const char *) NULL)
    {
      if (LocaleCompare(encoding,"AdobeCustom") == 0)
        encoding_type=FT_ENCODING_ADOBE_CUSTOM;
      if (LocaleCompare(encoding,"AdobeExpert") == 0)
        encoding_type=FT_ENCODING_ADOBE_EXPERT;
      if (LocaleCompare(encoding,"AdobeStandard") == 0)
        encoding_type=FT_ENCODING_ADOBE_STANDARD;
      if (LocaleCompare(encoding,"AppleRoman") == 0)
        encoding_type=FT_ENCODING_APPLE_ROMAN;
      if (LocaleCompare(encoding,"BIG5") == 0)
        encoding_type=FT_ENCODING_BIG5;
#if defined(FT_ENCODING_PRC)
      if (LocaleCompare(encoding,"GB2312") == 0)
        encoding_type=FT_ENCODING_PRC;
#endif
#if defined(FT_ENCODING_JOHAB)
      if (LocaleCompare(encoding,"Johab") == 0)
        encoding_type=FT_ENCODING_JOHAB;
#endif
#if defined(FT_ENCODING_ADOBE_LATIN_1)
      if (LocaleCompare(encoding,"Latin-1") == 0)
        encoding_type=FT_ENCODING_ADOBE_LATIN_1;
#endif
#if defined(FT_ENCODING_ADOBE_LATIN_2)
      if (LocaleCompare(encoding,"Latin-2") == 0)
        encoding_type=FT_ENCODING_OLD_LATIN_2;
#endif
      if (LocaleCompare(encoding,"None") == 0)
        encoding_type=FT_ENCODING_NONE;
      if (LocaleCompare(encoding,"SJIScode") == 0)
        encoding_type=FT_ENCODING_SJIS;
      if (LocaleCompare(encoding,"Symbol") == 0)
        encoding_type=FT_ENCODING_MS_SYMBOL;
      if (LocaleCompare(encoding,"Unicode") == 0)
        encoding_type=FT_ENCODING_UNICODE;
      if (LocaleCompare(encoding,"Wansung") == 0)
        encoding_type=FT_ENCODING_WANSUNG;
      ft_status=FT_Select_Charmap(face,encoding_type);
      if (ft_status != 0)
        {
          (void) FT_Done_Face(face);
          (void) FT_Done_FreeType(library);
          ThrowBinaryException(TypeError,"UnrecognizedFontEncoding",encoding);
        }
    }
  /*
    Set text size.
  */
  resolution.x=DefaultResolution;
  resolution.y=DefaultResolution;
  if (draw_info->density != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      MagickStatusType
        flags;

      flags=ParseGeometry(draw_info->density,&geometry_info);
      resolution.x=geometry_info.rho;
      resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        resolution.y=resolution.x;
    }
  ft_status=FT_Set_Char_Size(face,(FT_F26Dot6) (64.0*draw_info->pointsize),
    (FT_F26Dot6) (64.0*draw_info->pointsize),(FT_UInt) resolution.x,
    (FT_UInt) resolution.y);
  if (ft_status != 0)
    {
      (void) FT_Done_Face(face);
      (void) FT_Done_FreeType(library);
      ThrowBinaryException(TypeError,"UnableToReadFont",draw_info->font);
    }
  metrics->pixels_per_em.x=face->size->metrics.x_ppem;
  metrics->pixels_per_em.y=face->size->metrics.y_ppem;
  metrics->ascent=(double) face->size->metrics.ascender/64.0;
  metrics->descent=(double) face->size->metrics.descender/64.0;
  metrics->width=0;
  metrics->origin.x=0;
  metrics->origin.y=0;
  metrics->height=(double) face->size->metrics.height/64.0;
  metrics->max_advance=0.0;
  if (face->size->metrics.max_advance > MagickEpsilon)
    metrics->max_advance=(double) face->size->metrics.max_advance/64.0;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=face->underline_position/64.0;
  metrics->underline_thickness=face->underline_thickness/64.0;
  if ((draw_info->text == (char *) NULL) || (*draw_info->text == '\0'))
    {
      (void) FT_Done_Face(face);
      (void) FT_Done_FreeType(library);
      return(MagickTrue);
    }
  /*
    Compute bounding box.
  */
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),"Font %s; "
      "font-encoding %s; text-encoding %s; pointsize %g",
      draw_info->font != (char *) NULL ? draw_info->font : "none",
      encoding != (char *) NULL ? encoding : "none",
      draw_info->encoding != (char *) NULL ? draw_info->encoding : "none",
      draw_info->pointsize);
  flags=FT_LOAD_DEFAULT;
  if (draw_info->render == MagickFalse)
    flags=FT_LOAD_NO_BITMAP;
  if (draw_info->text_antialias == MagickFalse)
    flags|=FT_LOAD_TARGET_MONO;
  else
    {
#if defined(FT_LOAD_TARGET_LIGHT)
      flags|=FT_LOAD_TARGET_LIGHT;
#elif defined(FT_LOAD_TARGET_LCD)
      flags|=FT_LOAD_TARGET_LCD;
#endif
    }
  value=GetImageProperty(image,"type:hinting");
  if ((value != (const char *) NULL) && (LocaleCompare(value,"off") == 0))
    flags|=FT_LOAD_NO_HINTING;
  glyph.id=0;
  glyph.image=NULL;
  last_glyph.id=0;
  last_glyph.image=NULL;
  origin.x=0;
  origin.y=0;
  affine.xx=65536L;
  affine.yx=0L;
  affine.xy=0L;
  affine.yy=65536L;
  if (draw_info->render != MagickFalse)
    {
      affine.xx=(FT_Fixed) (65536L*draw_info->affine.sx+0.5);
      affine.yx=(FT_Fixed) (-65536L*draw_info->affine.rx+0.5);
      affine.xy=(FT_Fixed) (-65536L*draw_info->affine.ry+0.5);
      affine.yy=(FT_Fixed) (65536L*draw_info->affine.sy+0.5);
    }
  annotate_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  if (annotate_info->dash_pattern != (double *) NULL)
    annotate_info->dash_pattern[0]=0.0;
  (void) CloneString(&annotate_info->primitive,"path '");
  status=MagickTrue;
  if (draw_info->render != MagickFalse)
    {
      if (image->storage_class != DirectClass)
        (void) SetImageStorageClass(image,DirectClass);
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
    }
  point.x=0.0;
  point.y=0.0;
  for (p=draw_info->text; GetUTFCode(p) != 0; p+=GetUTFOctets(p))
    if (GetUTFCode(p) < 0)
      break;
  utf8=(unsigned char *) NULL;
  if (GetUTFCode(p) == 0)
    p=draw_info->text;
  else
    {
      utf8=ConvertLatin1ToUTF8((unsigned char *) draw_info->text);
      if (utf8 != (unsigned char *) NULL)
        p=(char *) utf8;
    }
  grapheme=(GraphemeInfo *) NULL;
  length=ComplexTextLayout(image,draw_info,p,strlen(p),face,flags,&grapheme);
  code=0;
  for (i=0; i < (ssize_t) length; i++)
  {
    FT_Outline
      outline;

    /*
      Render UTF-8 sequence.
    */
    glyph.id=grapheme[i].index;
    if (glyph.id == 0)
      glyph.id=FT_Get_Char_Index(face,' ');
    if ((glyph.id != 0) && (last_glyph.id != 0))
      origin.x+=(FT_Pos) (64.0*draw_info->kerning);
    glyph.origin=origin;
    glyph.origin.x+=grapheme[i].x_offset;
    glyph.origin.y+=grapheme[i].y_offset;
    glyph.image=0;
    ft_status=FT_Load_Glyph(face,glyph.id,flags);
    if (ft_status != 0)
      continue;
    ft_status=FT_Get_Glyph(face->glyph,&glyph.image);
    if (ft_status != 0)
      continue;
    outline=((FT_OutlineGlyph) glyph.image)->outline;
    ft_status=FT_Outline_Get_BBox(&outline,&bounds);
    if (ft_status != 0)
      continue;
    if ((p == draw_info->text) || (bounds.xMin < metrics->bounds.x1))
      if (bounds.xMin != 0)
        metrics->bounds.x1=(double) bounds.xMin;
    if ((p == draw_info->text) || (bounds.yMin < metrics->bounds.y1))
      if (bounds.yMin != 0)
        metrics->bounds.y1=(double) bounds.yMin;
    if ((p == draw_info->text) || (bounds.xMax > metrics->bounds.x2))
      if (bounds.xMax != 0)
        metrics->bounds.x2=(double) bounds.xMax;
    if ((p == draw_info->text) || (bounds.yMax > metrics->bounds.y2))
      if (bounds.yMax != 0)
        metrics->bounds.y2=(double) bounds.yMax;
    if (((draw_info->stroke.opacity != TransparentOpacity) ||
         (draw_info->stroke_pattern != (Image *) NULL)) &&
        ((status != MagickFalse) && (draw_info->render != MagickFalse)))
      {
        /*
          Trace the glyph.
        */
        annotate_info->affine.tx=glyph.origin.x/64.0;
        annotate_info->affine.ty=(-glyph.origin.y/64.0);
        if ((outline.n_contours > 0) && (outline.n_points > 0))
          ft_status=FT_Outline_Decompose(&outline,&OutlineMethods,
            annotate_info);
      }
    FT_Vector_Transform(&glyph.origin,&affine);
    (void) FT_Glyph_Transform(glyph.image,&affine,&glyph.origin);
    ft_status=FT_Glyph_To_Bitmap(&glyph.image,ft_render_mode_normal,
      (FT_Vector *) NULL,MagickTrue);
    if (ft_status != 0)
      continue;
    bitmap=(FT_BitmapGlyph) glyph.image;
    point.x=offset->x+bitmap->left;
    if (bitmap->bitmap.pixel_mode == ft_pixel_mode_mono)
      point.x=offset->x+(origin.x >> 6);
    point.y=offset->y-bitmap->top;
    if (draw_info->render != MagickFalse)
      {
        CacheView
          *image_view;

        register unsigned char
          *p;

        MagickBooleanType
          transparent_fill;

        /*
          Rasterize the glyph.
        */
        transparent_fill=((draw_info->fill.opacity == TransparentOpacity) &&
          (draw_info->fill_pattern == (Image *) NULL) &&
          (draw_info->stroke.opacity == TransparentOpacity) &&
          (draw_info->stroke_pattern == (Image *) NULL)) ? MagickTrue :
          MagickFalse;
        p=bitmap->bitmap.buffer;
        image_view=AcquireAuthenticCacheView(image,exception);
        for (y=0; y < (ssize_t) bitmap->bitmap.rows; y++)
        {
          MagickBooleanType
            active,
            sync;

          MagickRealType
            fill_opacity;

          PixelPacket
            fill_color;

          register PixelPacket
            *magick_restrict q;

          register ssize_t
            x;

          ssize_t
            n,
            x_offset,
            y_offset;

          if (status == MagickFalse)
            continue;
          x_offset=(ssize_t) ceil(point.x-0.5);
          y_offset=(ssize_t) ceil(point.y+y-0.5);
          if ((y_offset < 0) || (y_offset >= (ssize_t) image->rows))
            continue;
          q=(PixelPacket *) NULL;
          if ((x_offset < 0) || (x_offset >= (ssize_t) image->columns))
            active=MagickFalse;
          else
            {
              q=GetCacheViewAuthenticPixels(image_view,x_offset,y_offset,
                bitmap->bitmap.width,1,exception);
              active=q != (PixelPacket *) NULL ? MagickTrue : MagickFalse;
            }
          n=y*bitmap->bitmap.pitch-1;
          for (x=0; x < (ssize_t) bitmap->bitmap.width; x++)
          {
            n++;
            x_offset++;
            if ((x_offset < 0) || (x_offset >= (ssize_t) image->columns))
              {
                if (q != (PixelPacket *) NULL)
                  q++;
                continue;
              }
            if (bitmap->bitmap.pixel_mode != ft_pixel_mode_mono)
              fill_opacity=(MagickRealType) (p[n])/(bitmap->bitmap.num_grays-1);
            else
              fill_opacity=((p[(x >> 3)+y*bitmap->bitmap.pitch] &
                (1 << (~x & 0x07)))) == 0 ? 0.0 : 1.0;
            if (draw_info->text_antialias == MagickFalse)
              fill_opacity=fill_opacity >= 0.5 ? 1.0 : 0.0;
            if (active == MagickFalse)
              q=GetCacheViewAuthenticPixels(image_view,x_offset,y_offset,1,1,
                exception);
            if (q == (PixelPacket *) NULL)
              continue;
            if (transparent_fill == MagickFalse)
              {
                (void) GetFillColor(draw_info,x_offset,y_offset,&fill_color);
                fill_opacity=QuantumRange-fill_opacity*(QuantumRange-
                  fill_color.opacity);
                MagickCompositeOver(&fill_color,fill_opacity,q,q->opacity,q);
              }
            else
              {
                double
                  Sa,
                  Da;

                Da=1.0-(QuantumScale*(QuantumRange-q->opacity));
                Sa=fill_opacity;
                fill_opacity=(1.0-RoundToUnity(Sa+Da-Sa*Da))*QuantumRange;
                SetPixelAlpha(q,fill_opacity);
              }
            if (active == MagickFalse)
              {
                sync=SyncCacheViewAuthenticPixels(image_view,exception);
                if (sync == MagickFalse)
                  status=MagickFalse;
              }
            q++;
          }
          sync=SyncCacheViewAuthenticPixels(image_view,exception);
          if (sync == MagickFalse)
            status=MagickFalse;
        }
        image_view=DestroyCacheView(image_view);
        if (((draw_info->stroke.opacity != TransparentOpacity) ||
             (draw_info->stroke_pattern != (Image *) NULL)) &&
            (status != MagickFalse))
          {
            /*
              Draw text stroke.
            */
            annotate_info->linejoin=RoundJoin;
            annotate_info->affine.tx=offset->x;
            annotate_info->affine.ty=offset->y;
            (void) ConcatenateString(&annotate_info->primitive,"'");
            if (strlen(annotate_info->primitive) > 7)
              (void) DrawImage(image,annotate_info);
            (void) CloneString(&annotate_info->primitive,"path '");
          }
      }
    if ((fabs(draw_info->interword_spacing) >= MagickEpsilon) &&
        (IsUTFSpace(GetUTFCode(p+grapheme[i].cluster)) != MagickFalse) &&
        (IsUTFSpace(code) == MagickFalse))
      origin.x+=(FT_Pos) (64.0*draw_info->interword_spacing);
    else
      origin.x+=(FT_Pos) grapheme[i].x_advance;
    metrics->origin.x=(double) origin.x;
    metrics->origin.y=(double) origin.y;
    if (metrics->origin.x > metrics->width)
      metrics->width=metrics->origin.x;
    if (last_glyph.image != 0)
      {
        FT_Done_Glyph(last_glyph.image);
        last_glyph.image=0;
      }
    last_glyph=glyph;
    code=GetUTFCode(p+grapheme[i].cluster);
  }
  if (grapheme != (GraphemeInfo *) NULL)
    grapheme=(GraphemeInfo *) RelinquishMagickMemory(grapheme);
  if (utf8 != (unsigned char *) NULL)
    utf8=(unsigned char *) RelinquishMagickMemory(utf8);
  if (glyph.image != 0)
    {
      FT_Done_Glyph(glyph.image);
      glyph.image=0;
    }
  /*
    Determine font metrics.
  */
  metrics->bounds.x1/=64.0;
  metrics->bounds.y1/=64.0;
  metrics->bounds.x2/=64.0;
  metrics->bounds.y2/=64.0;
  metrics->origin.x/=64.0;
  metrics->origin.y/=64.0;
  metrics->width/=64.0;
  /*
    Relinquish resources.
  */
  annotate_info=DestroyDrawInfo(annotate_info);
  (void) FT_Done_Face(face);
  (void) FT_Done_FreeType(library);
  return(status);
}