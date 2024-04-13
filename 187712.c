static MagickBooleanType WriteSVGImage(const ImageInfo *image_info,Image *image)
{
#define BezierQuantum  200

  AffineMatrix
    affine;

  char
    keyword[MaxTextExtent],
    message[MaxTextExtent],
    name[MaxTextExtent],
    *next_token,
    *token,
    type[MaxTextExtent];

  const char
    *p,
    *q,
    *value;

  int
    n;

  ssize_t
    j;

  MagickBooleanType
    active,
    status;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register ssize_t
    x;

  register ssize_t
    i;

  size_t
    extent,
    length,
    number_points;

  SVGInfo
    svg_info;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  value=GetImageArtifact(image,"SVG");
  if (value != (char *) NULL)
    {
      (void) WriteBlobString(image,value);
      (void) CloseBlob(image);
      return(MagickTrue);
    }
  value=GetImageArtifact(image,"mvg:vector-graphics");
  if (value == (char *) NULL)
    return(TraceSVGImage(image,&image->exception));
  /*
    Write SVG header.
  */
  (void) WriteBlobString(image,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
  (void) WriteBlobString(image,
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"\n");
  (void) WriteBlobString(image,
    "  \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");
  (void) FormatLocaleString(message,MaxTextExtent,
    "<svg width=\"%.20g\" height=\"%.20g\">\n",(double) image->columns,(double)
    image->rows);
  (void) WriteBlobString(image,message);
  /*
    Allocate primitive info memory.
  */
  number_points=2047;
  primitive_info=(PrimitiveInfo *) AcquireQuantumMemory(number_points,
    sizeof(*primitive_info));
  if (primitive_info == (PrimitiveInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  GetAffineMatrix(&affine);
  token=AcquireString(value);
  extent=strlen(token)+MaxTextExtent;
  active=MagickFalse;
  n=0;
  status=MagickTrue;
  for (q=(const char *) value; *q != '\0'; )
  {
    /*
      Interpret graphic primitive.
    */
    (void) GetNextToken(q,&q,extent,keyword);
    if (*keyword == '\0')
      break;
    if (*keyword == '#')
      {
        /*
          Comment.
        */
        if (active != MagickFalse)
          {
            AffineToTransform(image,&affine);
            active=MagickFalse;
          }
        (void) WriteBlobString(image,"<desc>");
        (void) WriteBlobString(image,keyword+1);
        for ( ; (*q != '\n') && (*q != '\0'); q++)
          switch (*q)
          {
            case '<': (void) WriteBlobString(image,"&lt;"); break;
            case '>': (void) WriteBlobString(image,"&gt;"); break;
            case '&': (void) WriteBlobString(image,"&amp;"); break;
            default: (void) WriteBlobByte(image,(unsigned char) *q); break;
          }
        (void) WriteBlobString(image,"</desc>\n");
        continue;
      }
    primitive_type=UndefinedPrimitive;
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            affine.sx=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.rx=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.ry=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.sy=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.tx=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.ty=StringToDouble(token,&next_token);
            break;
          }
        if (LocaleCompare("angle",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            affine.rx=StringToDouble(token,&next_token);
            affine.ry=StringToDouble(token,&next_token);
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'b':
      case 'B':
      {
        if (LocaleCompare("bezier",keyword) == 0)
          {
            primitive_type=BezierPrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'c':
      case 'C':
      {
        if (LocaleCompare("clip-path",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "clip-path:url(#%s);",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("clip-rule",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"clip-rule:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("clip-units",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"clipPathUnits=%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("circle",keyword) == 0)
          {
            primitive_type=CirclePrimitive;
            break;
          }
        if (LocaleCompare("color",keyword) == 0)
          {
            primitive_type=ColorPrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "text-decoration:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'e':
      case 'E':
      {
        if (LocaleCompare("ellipse",keyword) == 0)
          {
            primitive_type=EllipsePrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"fill:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("fill-rule",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"fill-rule:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"fill-opacity:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font-family",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"font-family:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font-stretch",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"font-stretch:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font-style",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"font-style:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font-size",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"font-size:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font-weight",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"font-weight:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gradient-units",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            break;
          }
        if (LocaleCompare("text-align",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"text-align %s ",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("text-anchor",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent, "text-anchor %s ",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            primitive_type=ImagePrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'k':
      case 'K':
      {
        if (LocaleCompare("kerning",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MagickPathExtent,"kerning:%s;",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        break;
      }
      case 'l':
      case 'L':
      {
        if (LocaleCompare("letter-spacing",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MagickPathExtent,
              "letter-spacing:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("line",keyword) == 0)
          {
            primitive_type=LinePrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'm':
      case 'M':
      {
        if (LocaleCompare("matte",keyword) == 0)
          {
            primitive_type=MattePrimitive;
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("opacity",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"opacity %s ",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'p':
      case 'P':
      {
        if (LocaleCompare("path",keyword) == 0)
          {
            primitive_type=PathPrimitive;
            break;
          }
        if (LocaleCompare("point",keyword) == 0)
          {
            primitive_type=PointPrimitive;
            break;
          }
        if (LocaleCompare("polyline",keyword) == 0)
          {
            primitive_type=PolylinePrimitive;
            break;
          }
        if (LocaleCompare("polygon",keyword) == 0)
          {
            primitive_type=PolygonPrimitive;
            break;
          }
        if (LocaleCompare("pop",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            if (LocaleCompare("clip-path",token) == 0)
              {
                (void) WriteBlobString(image,"</clipPath>\n");
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              {
                (void) WriteBlobString(image,"</defs>\n");
                break;
              }
            if (LocaleCompare("gradient",token) == 0)
              {
                (void) FormatLocaleString(message,MaxTextExtent,
                  "</%sGradient>\n",type);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n--;
                if (n < 0)
                  ThrowWriterException(DrawError,
                    "UnbalancedGraphicContextPushPop");
                (void) WriteBlobString(image,"</g>\n");
              }
            if (LocaleCompare("pattern",token) == 0)
              {
                (void) WriteBlobString(image,"</pattern>\n");
                break;
              }
            if (LocaleCompare("symbol",token) == 0)
              {
                (void) WriteBlobString(image,"</symbol>\n");
                break;
              }
            if ((LocaleCompare("defs",token) == 0) ||
                (LocaleCompare("symbol",token) == 0))
              (void) WriteBlobString(image,"</g>\n");
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            if (LocaleCompare("clip-path",token) == 0)
              {
                (void) GetNextToken(q,&q,extent,token);
                (void) FormatLocaleString(message,MaxTextExtent,
                  "<clipPath id=\"%s\">\n",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              {
                (void) WriteBlobString(image,"<defs>\n");
                break;
              }
            if (LocaleCompare("gradient",token) == 0)
              {
                (void) GetNextToken(q,&q,extent,token);
                (void) CopyMagickString(name,token,MaxTextExtent);
                (void) GetNextToken(q,&q,extent,token);
                (void) CopyMagickString(type,token,MaxTextExtent);
                (void) GetNextToken(q,&q,extent,token);
                svg_info.segment.x1=StringToDouble(token,&next_token);
                svg_info.element.cx=StringToDouble(token,&next_token);
                (void) GetNextToken(q,&q,extent,token);
                if (*token == ',')
                  (void) GetNextToken(q,&q,extent,token);
                svg_info.segment.y1=StringToDouble(token,&next_token);
                svg_info.element.cy=StringToDouble(token,&next_token);
                (void) GetNextToken(q,&q,extent,token);
                if (*token == ',')
                  (void) GetNextToken(q,&q,extent,token);
                svg_info.segment.x2=StringToDouble(token,&next_token);
                svg_info.element.major=StringToDouble(token,
                  (char **) NULL);
                (void) GetNextToken(q,&q,extent,token);
                if (*token == ',')
                  (void) GetNextToken(q,&q,extent,token);
                svg_info.segment.y2=StringToDouble(token,&next_token);
                svg_info.element.minor=StringToDouble(token,
                  (char **) NULL);
                (void) FormatLocaleString(message,MaxTextExtent,
                  "<%sGradient id=\"%s\" x1=\"%g\" y1=\"%g\" x2=\"%g\" "
                  "y2=\"%g\">\n",type,name,svg_info.segment.x1,
                  svg_info.segment.y1,svg_info.segment.x2,svg_info.segment.y2);
                if (LocaleCompare(type,"radial") == 0)
                  {
                    (void) GetNextToken(q,&q,extent,token);
                    if (*token == ',')
                      (void) GetNextToken(q,&q,extent,token);
                    svg_info.element.angle=StringToDouble(token,
                      (char **) NULL);
                    (void) FormatLocaleString(message,MaxTextExtent,
                      "<%sGradient id=\"%s\" cx=\"%g\" cy=\"%g\" r=\"%g\" "
                      "fx=\"%g\" fy=\"%g\">\n",type,name,
                      svg_info.element.cx,svg_info.element.cy,
                      svg_info.element.angle,svg_info.element.major,
                      svg_info.element.minor);
                  }
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n++;
                if (active)
                  {
                    AffineToTransform(image,&affine);
                    active=MagickFalse;
                  }
                (void) WriteBlobString(image,"<g style=\"");
                active=MagickTrue;
              }
            if (LocaleCompare("pattern",token) == 0)
              {
                (void) GetNextToken(q,&q,extent,token);
                (void) CopyMagickString(name,token,MaxTextExtent);
                (void) GetNextToken(q,&q,extent,token);
                svg_info.bounds.x=StringToDouble(token,&next_token);
                (void) GetNextToken(q,&q,extent,token);
                if (*token == ',')
                  (void) GetNextToken(q,&q,extent,token);
                svg_info.bounds.y=StringToDouble(token,&next_token);
                (void) GetNextToken(q,&q,extent,token);
                if (*token == ',')
                  (void) GetNextToken(q,&q,extent,token);
                svg_info.bounds.width=StringToDouble(token,
                  (char **) NULL);
                (void) GetNextToken(q,&q,extent,token);
                if (*token == ',')
                  (void) GetNextToken(q,&q,extent,token);
                svg_info.bounds.height=StringToDouble(token,(char **) NULL);
                (void) FormatLocaleString(message,MaxTextExtent,
                  "<pattern id=\"%s\" x=\"%g\" y=\"%g\" width=\"%g\" "
                  "height=\"%g\">\n",name,svg_info.bounds.x,svg_info.bounds.y,
                  svg_info.bounds.width,svg_info.bounds.height);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("symbol",token) == 0)
              {
                (void) WriteBlobString(image,"<symbol>\n");
                break;
              }
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleCompare("rectangle",keyword) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"rotate(%s) ",
              token);
            (void) WriteBlobString(image,message);
            break;
          }
        status=MagickFalse;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            affine.sx=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.sy=StringToDouble(token,&next_token);
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"skewX(%s) ",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"skewY(%s) ",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stop-color",keyword) == 0)
          {
            char
              color[MaxTextExtent];

            (void) GetNextToken(q,&q,extent,token);
            (void) CopyMagickString(color,token,MaxTextExtent);
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "  <stop offset=\"%s\" stop-color=\"%s\" />\n",token,color);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"stroke:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-antialias:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            if (IsPoint(q))
              {
                ssize_t
                  k;

                p=q;
                (void) GetNextToken(p,&p,extent,token);
                for (k=0; IsPoint(token); k++)
                  (void) GetNextToken(p,&p,extent,token);
                (void) WriteBlobString(image,"stroke-dasharray:");
                for (j=0; j < k; j++)
                {
                  (void) GetNextToken(q,&q,extent,token);
                  (void) FormatLocaleString(message,MaxTextExtent,"%s ",token);
                  (void) WriteBlobString(image,message);
                }
                (void) WriteBlobString(image,";");
                break;
              }
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-dasharray:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-dashoffset:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-linecap:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-linejoin:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-miterlimit:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "stroke-opacity:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,"stroke-width:%s;",
              token);
            (void) WriteBlobString(image,message);
            continue;
          }
        status=MagickFalse;
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare("text",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            break;
          }
        if (LocaleCompare("text-antialias",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            (void) FormatLocaleString(message,MaxTextExtent,
              "text-antialias:%s;",token);
            (void) WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("tspan",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            affine.tx=StringToDouble(token,&next_token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            affine.ty=StringToDouble(token,&next_token);
            break;
          }
        status=MagickFalse;
        break;
      }
      case 'v':
      case 'V':
      {
        if (LocaleCompare("viewbox",keyword) == 0)
          {
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            (void) GetNextToken(q,&q,extent,token);
            if (*token == ',')
              (void) GetNextToken(q,&q,extent,token);
            (void) GetNextToken(q,&q,extent,token);
            break;
          }
        status=MagickFalse;
        break;
      }
      default:
      {
        status=MagickFalse;
        break;
      }
    }
    if (status == MagickFalse)
      break;
    if (primitive_type == UndefinedPrimitive)
      continue;
    /*
      Parse the primitive attributes.
    */
    i=0;
    j=0;
    for (x=0; *q != '\0'; x++)
    {
      /*
        Define points.
      */
      if (IsPoint(q) == MagickFalse)
        break;
      (void) GetNextToken(q,&q,extent,token);
      point.x=StringToDouble(token,&next_token);
      (void) GetNextToken(q,&q,extent,token);
      if (*token == ',')
        (void) GetNextToken(q,&q,extent,token);
      point.y=StringToDouble(token,&next_token);
      (void) GetNextToken(q,(const char **) NULL,extent,token);
      if (*token == ',')
        (void) GetNextToken(q,&q,extent,token);
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      i++;
      if (i < (ssize_t) (number_points-6*BezierQuantum-360))
        continue;
      number_points+=6*BezierQuantum+360;
      primitive_info=(PrimitiveInfo *) ResizeQuantumMemory(primitive_info,
        number_points,sizeof(*primitive_info));
      if (primitive_info == (PrimitiveInfo *) NULL)
        {
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
          break;
        }
    }
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=(size_t) x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    if (active)
      {
        AffineToTransform(image,&affine);
        active=MagickFalse;
      }
    active=MagickFalse;
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=MagickFalse;
            break;
          }
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFalse;
            break;
          }
          (void) FormatLocaleString(message,MaxTextExtent,
          "  <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x,primitive_info[j+1].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFalse;
            break;
          }
          (void) FormatLocaleString(message,MaxTextExtent,
          "  <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x-primitive_info[j].point.x,
          primitive_info[j+1].point.y-primitive_info[j].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=MagickFalse;
            break;
          }
        (void) FormatLocaleString(message,MaxTextExtent,
          "  <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" rx=\"%g\" "
          "ry=\"%g\"/>\n",primitive_info[j].point.x,
          primitive_info[j].point.y,primitive_info[j+1].point.x-
          primitive_info[j].point.x,primitive_info[j+1].point.y-
          primitive_info[j].point.y,primitive_info[j+2].point.x,
          primitive_info[j+2].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=MagickFalse;
            break;
          }
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=MagickFalse;
            break;
          }
          (void) FormatLocaleString(message,MaxTextExtent,
          "  <ellipse cx=\"%g\" cy=\"%g\" rx=\"%g\" ry=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x,primitive_info[j+1].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case CirclePrimitive:
      {
        double
          alpha,
          beta;

        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFalse;
            break;
          }
        alpha=primitive_info[j+1].point.x-primitive_info[j].point.x;
        beta=primitive_info[j+1].point.y-primitive_info[j].point.y;
        (void) FormatLocaleString(message,MaxTextExtent,
          "  <circle cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          hypot(alpha,beta));
        (void) WriteBlobString(image,message);
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            status=MagickFalse;
            break;
          }
        (void) CopyMagickString(message,"  <polyline points=\"",MaxTextExtent);
        (void) WriteBlobString(image,message);
        length=strlen(message);
        for ( ; j < i; j++)
        {
          (void) FormatLocaleString(message,MaxTextExtent,"%g,%g ",
            primitive_info[j].point.x,primitive_info[j].point.y);
          length+=strlen(message);
          if (length >= 80)
            {
              (void) WriteBlobString(image,"\n    ");
              length=strlen(message)+5;
            }
          (void) WriteBlobString(image,message);
        }
        (void) WriteBlobString(image,"\"/>\n");
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=MagickFalse;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        (void) CopyMagickString(message,"  <polygon points=\"",MaxTextExtent);
        (void) WriteBlobString(image,message);
        length=strlen(message);
        for ( ; j < i; j++)
        {
          (void) FormatLocaleString(message,MaxTextExtent,"%g,%g ",
            primitive_info[j].point.x,primitive_info[j].point.y);
          length+=strlen(message);
          if (length >= 80)
            {
              (void) WriteBlobString(image,"\n    ");
              length=strlen(message)+5;
            }
          (void) WriteBlobString(image,message);
        }
        (void) WriteBlobString(image,"\"/>\n");
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=MagickFalse;
            break;
          }
        break;
      }
      case PathPrimitive:
      {
        int
          number_attributes;

        (void) GetNextToken(q,&q,extent,token);
        number_attributes=1;
        for (p=token; *p != '\0'; p++)
          if (isalpha((int) *p))
            number_attributes++;
        if (i > (ssize_t) (number_points-6*BezierQuantum*number_attributes-1))
          {
            number_points+=6*BezierQuantum*number_attributes;
            primitive_info=(PrimitiveInfo *) ResizeQuantumMemory(primitive_info,
              number_points,sizeof(*primitive_info));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                (void) ThrowMagickException(&image->exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed","`%s'",
                  image->filename);
                break;
              }
          }
        (void) WriteBlobString(image,"  <path d=\"");
        (void) WriteBlobString(image,token);
        (void) WriteBlobString(image,"\"/>\n");
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=MagickFalse;
            break;
          }
        (void) GetNextToken(q,&q,extent,token);
        if (LocaleCompare("point",token) == 0)
          primitive_info[j].method=PointMethod;
        if (LocaleCompare("replace",token) == 0)
          primitive_info[j].method=ReplaceMethod;
        if (LocaleCompare("floodfill",token) == 0)
          primitive_info[j].method=FloodfillMethod;
        if (LocaleCompare("filltoborder",token) == 0)
          primitive_info[j].method=FillToBorderMethod;
        if (LocaleCompare("reset",token) == 0)
          primitive_info[j].method=ResetMethod;
        break;
      }
      case TextPrimitive:
      {
        register char
          *p;

        if (primitive_info[j].coordinates != 1)
          {
            status=MagickFalse;
            break;
          }
        (void) GetNextToken(q,&q,extent,token);
        (void) FormatLocaleString(message,MaxTextExtent,
          "  <text x=\"%g\" y=\"%g\">",primitive_info[j].point.x,
          primitive_info[j].point.y);
        (void) WriteBlobString(image,message);
        for (p=token; *p != '\0'; p++)
          switch (*p)
          {
            case '<': (void) WriteBlobString(image,"&lt;"); break;
            case '>': (void) WriteBlobString(image,"&gt;"); break;
            case '&': (void) WriteBlobString(image,"&amp;"); break;
            default: (void) WriteBlobByte(image,(unsigned char) *p); break;
          }
        (void) WriteBlobString(image,"</text>\n");
        break;
      }
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFalse;
            break;
          }
        (void) GetNextToken(q,&q,extent,token);
        (void) FormatLocaleString(message,MaxTextExtent,
          "  <image x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" "
          "href=\"%s\"/>\n",primitive_info[j].point.x,
          primitive_info[j].point.y,primitive_info[j+1].point.x,
          primitive_info[j+1].point.y,token);
        (void) WriteBlobString(image,message);
        break;
      }
    }
    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    if (status == MagickFalse)
      break;
  }
  (void) WriteBlobString(image,"</svg>\n");
  /*
    Relinquish resources.
  */
  token=DestroyString(token);
  if (primitive_info != (PrimitiveInfo *) NULL)
    primitive_info=(PrimitiveInfo *) RelinquishMagickMemory(primitive_info);
  (void) CloseBlob(image);
  return(status);
}