static void SVGStartElement(void *context,const xmlChar *name,
  const xmlChar **attributes)
{
#define PushGraphicContext(id) \
{ \
  if (*id == '\0') \
    (void) FormatLocaleFile(svg_info->file,"push graphic-context\n"); \
  else \
    (void) FormatLocaleFile(svg_info->file,"push graphic-context \"%s\"\n", \
      id); \
}

  char
    *color,
    background[MagickPathExtent],
    id[MaxTextExtent],
    *next_token,
    token[MaxTextExtent],
    **tokens,
    *units;

  const char
    *keyword,
    *p,
    *value;

  register ssize_t
    i,
    j;

  size_t
    number_tokens;

  SVGInfo
    *svg_info;

  /*
    Called when an opening tag has been processed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.startElement(%s",
    name);
  svg_info=(SVGInfo *) context;
  svg_info->n++;
  svg_info->scale=(double *) ResizeQuantumMemory(svg_info->scale,
    svg_info->n+1UL,sizeof(*svg_info->scale));
  if (svg_info->scale == (double *) NULL)
    {
      (void) ThrowMagickException(svg_info->exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",name);
      return;
    }
  svg_info->scale[svg_info->n]=svg_info->scale[svg_info->n-1];
  color=AcquireString("none");
  units=AcquireString("userSpaceOnUse");
  *id='\0';
  *token='\0';
  *background='\0';
  value=(const char *) NULL;
  if ((LocaleCompare((char *) name,"image") == 0) ||
      (LocaleCompare((char *) name,"pattern") == 0) ||
      (LocaleCompare((char *) name,"rect") == 0) ||
      (LocaleCompare((char *) name,"text") == 0) ||
      (LocaleCompare((char *) name,"use") == 0))
    {
      svg_info->bounds.x=0.0;
      svg_info->bounds.y=0.0;
    }
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(const char *) attributes[i];
      value=(const char *) attributes[i+1];
      switch (*keyword)
      {
        case 'C':
        case 'c':
        {
          if (LocaleCompare(keyword,"cx") == 0)
            {
              svg_info->element.cx=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"cy") == 0)
            {
              svg_info->element.cy=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (LocaleCompare(keyword,"fx") == 0)
            {
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"fy") == 0)
            {
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (LocaleCompare(keyword,"height") == 0)
            {
              svg_info->bounds.height=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        case 'I':
        case 'i':
        {
          if (LocaleCompare(keyword,"id") == 0)
            {
              (void) CopyMagickString(id,value,MaxTextExtent);
              break;
            }
          break;
        }
        case 'R':
        case 'r':
        {
          if (LocaleCompare(keyword,"r") == 0)
            {
              svg_info->element.angle=
                GetUserSpaceCoordinateValue(svg_info,0,value);
              break;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (LocaleCompare(keyword,"width") == 0)
            {
              svg_info->bounds.width=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          break;
        }
        case 'X':
        case 'x':
        {
          if (LocaleCompare(keyword,"x") == 0)
            {
              if (LocaleCompare((char *) name,"tspan") != 0)
                svg_info->bounds.x=GetUserSpaceCoordinateValue(svg_info,1,
                  value)-svg_info->center.x;
              break;
            }
          if (LocaleCompare(keyword,"x1") == 0)
            {
              svg_info->segment.x1=GetUserSpaceCoordinateValue(svg_info,1,
                value);
              break;
            }
          if (LocaleCompare(keyword,"x2") == 0)
            {
              svg_info->segment.x2=GetUserSpaceCoordinateValue(svg_info,1,
                value);
              break;
            }
          break;
        }
        case 'Y':
        case 'y':
        {
          if (LocaleCompare(keyword,"y") == 0)
            {
              if (LocaleCompare((char *) name,"tspan") != 0)
                svg_info->bounds.y=GetUserSpaceCoordinateValue(svg_info,-1,
                  value)-svg_info->center.y;
              break;
            }
          if (LocaleCompare(keyword,"y1") == 0)
            {
              svg_info->segment.y1=GetUserSpaceCoordinateValue(svg_info,-1,
                value);
              break;
            }
          if (LocaleCompare(keyword,"y2") == 0)
            {
              svg_info->segment.y2=GetUserSpaceCoordinateValue(svg_info,-1,
                value);
              break;
            }
          break;
        }
        default:
          break;
      }
    }
  if (strchr((char *) name,':') != (char *) NULL)
    {
      /*
        Skip over namespace.
      */
      for ( ; *name != ':'; name++) ;
      name++;
    }
  switch (*name)
  {
    case 'C':
    case 'c':
    {
      if (LocaleCompare((const char *) name,"circle") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      if (LocaleCompare((const char *) name,"clipPath") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,"push clip-path \"%s\"\n",id);
          break;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare((const char *) name,"defs") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,"push defs\n");
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleCompare((const char *) name,"ellipse") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'F':
    case 'f':
    {
      if (LocaleCompare((const char *) name,"foreignObject") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare((const char *) name,"g") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((const char *) name,"image") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare((const char *) name,"line") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      if (LocaleCompare((const char *) name,"linearGradient") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,
            "push gradient \"%s\" linear %g,%g %g,%g\n",id,
            svg_info->segment.x1,svg_info->segment.y1,svg_info->segment.x2,
            svg_info->segment.y2);
          break;
        }
      break;
    }
    case 'M':
    case 'm':
    {
      if (LocaleCompare((const char *) name,"mask") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,"push mask \"%s\"\n",id);
          break;
        }
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare((const char *) name,"path") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      if (LocaleCompare((const char *) name,"pattern") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,
            "push pattern \"%s\" %g,%g %g,%g\n",id,
            svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.width,
            svg_info->bounds.height);
          break;
        }
      if (LocaleCompare((const char *) name,"polygon") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      if (LocaleCompare((const char *) name,"polyline") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare((const char *) name,"radialGradient") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,
            "push gradient \"%s\" radial %g,%g %g,%g %g\n",
            id,svg_info->element.cx,svg_info->element.cy,
            svg_info->element.major,svg_info->element.minor,
            svg_info->element.angle);
          break;
        }
      if (LocaleCompare((const char *) name,"rect") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'S':
    case 's':
    {
      if (LocaleCompare((char *) name,"style") == 0)
        break;
      if (LocaleCompare((const char *) name,"svg") == 0)
        {
          svg_info->svgDepth++;
          PushGraphicContext(id);
          (void) FormatLocaleFile(svg_info->file,"compliance \"SVG\"\n");
          (void) FormatLocaleFile(svg_info->file,"fill \"black\"\n");
          (void) FormatLocaleFile(svg_info->file,"fill-opacity 1\n");
          (void) FormatLocaleFile(svg_info->file,"stroke \"none\"\n");
          (void) FormatLocaleFile(svg_info->file,"stroke-width 1\n");
          (void) FormatLocaleFile(svg_info->file,"stroke-opacity 1\n");
          (void) FormatLocaleFile(svg_info->file,"fill-rule nonzero\n");
          break;
        }
      if (LocaleCompare((const char *) name,"symbol") == 0)
        {
          (void) FormatLocaleFile(svg_info->file,"push symbol\n");
          break;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare((const char *) name,"text") == 0)
        {
          PushGraphicContext(id);
          (void) FormatLocaleFile(svg_info->file,"class \"text\"\n");
          (void) FormatLocaleFile(svg_info->file,"translate %g,%g\n",
            svg_info->bounds.x,svg_info->bounds.y);
          svg_info->center.x=svg_info->bounds.x;
          svg_info->center.y=svg_info->bounds.y;
          svg_info->bounds.x=0.0;
          svg_info->bounds.y=0.0;
          svg_info->bounds.width=0.0;
          svg_info->bounds.height=0.0;
          break;
        }
      if (LocaleCompare((const char *) name,"tspan") == 0)
        {
          if (*svg_info->text != '\0')
            {
              char
                *text;

              text=EscapeString(svg_info->text,'\"');
              (void) FormatLocaleFile(svg_info->file,"text %g,%g \"%s\"\n",
                svg_info->bounds.x-svg_info->center.x,svg_info->bounds.y-
                svg_info->center.y,text);
              text=DestroyString(text);
              *svg_info->text='\0';
            }
          PushGraphicContext(id);
          break;
        }
      break;
    }
    case 'U':
    case 'u':
    {
      if (LocaleCompare((char *) name,"use") == 0)
        {
          PushGraphicContext(id);
          break;
        }
      break;
    }
    default:
      break;
  }
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(const char *) attributes[i];
      value=(const char *) attributes[i+1];
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    %s = %s",keyword,value);
      switch (*keyword)
      {
        case 'A':
        case 'a':
        {
          if (LocaleCompare(keyword,"angle") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"angle %g\n",
                GetUserSpaceCoordinateValue(svg_info,0,value));
              break;
            }
          break;
        }
        case 'C':
        case 'c':
        {
          if (LocaleCompare(keyword,"class") == 0)
            {
              const char
                *p;

              for (p=value; ; )
              {
                GetNextToken(p,&p,MagickPathExtent,token);
                if (*token == ',')
                  GetNextToken(p,&p,MagickPathExtent,token);
                if (*token != '\0')
                  {
                    (void) FormatLocaleFile(svg_info->file,"class \"%s\"\n",
                      value);
                    break;
                  }
              }
              break;
            }
          if (LocaleCompare(keyword,"clip-path") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"clip-path \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"clip-rule") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"clip-rule \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"clipPathUnits") == 0)
            {
              (void) CloneString(&units,value);
              (void) FormatLocaleFile(svg_info->file,"clip-units \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"color") == 0)
            {
              (void) CloneString(&color,value);
              break;
            }
          if (LocaleCompare(keyword,"cx") == 0)
            {
              svg_info->element.cx=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"cy") == 0)
            {
              svg_info->element.cy=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        case 'D':
        case 'd':
        {
          if (LocaleCompare(keyword,"d") == 0)
            {
              (void) CloneString(&svg_info->vertices,value);
              break;
            }
          if (LocaleCompare(keyword,"dx") == 0)
            {
              double
                dx;

              dx=GetUserSpaceCoordinateValue(svg_info,1,value);
              svg_info->bounds.x+=dx;
              if (LocaleCompare((char *) name,"text") == 0)
                (void) FormatLocaleFile(svg_info->file,"translate %g,0.0\n",dx);
              break;
            }
          if (LocaleCompare(keyword,"dy") == 0)
            {
              double
                dy;

              dy=GetUserSpaceCoordinateValue(svg_info,-1,value);
              svg_info->bounds.y+=dy;
              if (LocaleCompare((char *) name,"text") == 0)
                (void) FormatLocaleFile(svg_info->file,"translate 0.0,%g\n",dy);
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (LocaleCompare(keyword,"fill") == 0)
            {
              if (LocaleCompare(value,"currentColor") == 0)
                {
                  (void) FormatLocaleFile(svg_info->file,"fill \"%s\"\n",color);
                  break;
                }
              (void) FormatLocaleFile(svg_info->file,"fill \"%s\"\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fillcolor") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"fill \"%s\"\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fill-rule") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"fill-rule \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"fill-opacity") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"fill-opacity \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"font-family") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"font-family \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"font-stretch") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"font-stretch \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"font-style") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"font-style \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"font-size") == 0)
            {
              if (LocaleCompare(value,"xx-small") == 0)
                svg_info->pointsize=6.144;
              else if (LocaleCompare(value,"x-small") == 0)
                svg_info->pointsize=7.68;
              else if (LocaleCompare(value,"small") == 0)
                svg_info->pointsize=9.6;
              else if (LocaleCompare(value,"medium") == 0)
                svg_info->pointsize=12.0;
              else if (LocaleCompare(value,"large") == 0)
                svg_info->pointsize=14.4;
              else if (LocaleCompare(value,"x-large") == 0)
                svg_info->pointsize=17.28;
              else if (LocaleCompare(value,"xx-large") == 0)
                svg_info->pointsize=20.736;
              else
                svg_info->pointsize=GetUserSpaceCoordinateValue(svg_info,0,
                  value);
              (void) FormatLocaleFile(svg_info->file,"font-size %g\n",
                svg_info->pointsize);
              break;
            }
          if (LocaleCompare(keyword,"font-weight") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"font-weight \"%s\"\n",
                value);
              break;
            }
          break;
        }
        case 'G':
        case 'g':
        {
          if (LocaleCompare(keyword,"gradientTransform") == 0)
            {
              AffineMatrix
                affine,
                current,
                transform;

              GetAffineMatrix(&transform);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  ");
              tokens=SVGKeyValuePairs(context,'(',')',value,&number_tokens);
              if (tokens == (char **) NULL)
                break;
              for (j=0; j < (ssize_t) (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                if (keyword == (char *) NULL)
                  continue;
                value=(char *) tokens[j+1];
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    %s: %s",keyword,value);
                current=transform;
                GetAffineMatrix(&affine);
                switch (*keyword)
                {
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare(keyword,"matrix") == 0)
                      {
                        p=(const char *) value;
                        GetNextToken(p,&p,MaxTextExtent,token);
                        affine.sx=StringToDouble(value,(char **) NULL);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.rx=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.ry=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.sy=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.tx=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.ty=StringToDouble(token,&next_token);
                        break;
                      }
                    break;
                  }
                  case 'R':
                  case 'r':
                  {
                    if (LocaleCompare(keyword,"rotate") == 0)
                      {
                        double
                          angle;

                        angle=GetUserSpaceCoordinateValue(svg_info,0,value);
                        affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                        affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                        affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                        affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
                    if (LocaleCompare(keyword,"scale") == 0)
                      {
                        for (p=(const char *) value; *p != '\0'; p++)
                          if ((isspace((int) ((unsigned char) *p)) != 0) ||
                              (*p == ','))
                            break;
                        affine.sx=GetUserSpaceCoordinateValue(svg_info,1,value);
                        affine.sy=affine.sx;
                        if (*p != '\0')
                          affine.sy=
                            GetUserSpaceCoordinateValue(svg_info,-1,p+1);
                        svg_info->scale[svg_info->n]=ExpandAffine(&affine);
                        break;
                      }
                    if (LocaleCompare(keyword,"skewX") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.ry=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,1,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    if (LocaleCompare(keyword,"skewY") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.rx=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,-1,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    break;
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare(keyword,"translate") == 0)
                      {
                        for (p=(const char *) value; *p != '\0'; p++)
                          if ((isspace((int) ((unsigned char) *p)) != 0) ||
                              (*p == ','))
                            break;
                        affine.tx=GetUserSpaceCoordinateValue(svg_info,1,value);
                        affine.ty=0.0;
                        if (*p != '\0')
                          affine.ty=GetUserSpaceCoordinateValue(svg_info,-1,
                            p+1);
                        break;
                      }
                    break;
                  }
                  default:
                    break;
                }
                transform.sx=affine.sx*current.sx+affine.ry*current.rx;
                transform.rx=affine.rx*current.sx+affine.sy*current.rx;
                transform.ry=affine.sx*current.ry+affine.ry*current.sy;
                transform.sy=affine.rx*current.ry+affine.sy*current.sy;
                transform.tx=affine.tx*current.sx+affine.ty*current.ry+
                  current.tx;
                transform.ty=affine.tx*current.rx+affine.ty*current.sy+
                  current.ty;
              }
              (void) FormatLocaleFile(svg_info->file,
                "affine %g %g %g %g %g %g\n",transform.sx,
                transform.rx,transform.ry,transform.sy,transform.tx,
                transform.ty);
              for (j=0; tokens[j] != (char *) NULL; j++)
                tokens[j]=DestroyString(tokens[j]);
              tokens=(char **) RelinquishMagickMemory(tokens);
              break;
            }
          if (LocaleCompare(keyword,"gradientUnits") == 0)
            {
              (void) CloneString(&units,value);
              (void) FormatLocaleFile(svg_info->file,"gradient-units \"%s\"\n",
                value);
              break;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (LocaleCompare(keyword,"height") == 0)
            {
              svg_info->bounds.height=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          if (LocaleCompare(keyword,"href") == 0)
            {
              (void) CloneString(&svg_info->url,value);
              break;
            }
          break;
        }
        case 'K':
        case 'k':
        {
          if (LocaleCompare(keyword,"kernel") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"kernel \"%s\"\n",value);
              break;
            }
          break;
        }
        case 'L':
        case 'l':
        {
          if (LocaleCompare(keyword,"letter-spacing") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"letter-spacing \"%s\"\n",
                value);
              break;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (LocaleCompare(keyword,"major") == 0)
            {
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"mask") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"mask \"%s\"\n",value);
              break;
            }
          if (LocaleCompare(keyword,"minor") == 0)
            {
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        case 'O':
        case 'o':
        {
          if (LocaleCompare(keyword,"offset") == 0)
            {
              (void) CloneString(&svg_info->offset,value);
              break;
            }
          if (LocaleCompare(keyword,"opacity") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"opacity \"%s\"\n",value);
              break;
            }
          break;
        }
        case 'P':
        case 'p':
        {
          if (LocaleCompare(keyword,"path") == 0)
            {
              (void) CloneString(&svg_info->url,value);
              break;
            }
          if (LocaleCompare(keyword,"points") == 0)
            {
              (void) CloneString(&svg_info->vertices,value);
              break;
            }
          break;
        }
        case 'R':
        case 'r':
        {
          if (LocaleCompare(keyword,"r") == 0)
            {
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          if (LocaleCompare(keyword,"rotate") == 0)
            {
              double
                angle;

              angle=GetUserSpaceCoordinateValue(svg_info,0,value);
              (void) FormatLocaleFile(svg_info->file,"translate %g,%g\n",
                svg_info->bounds.x,svg_info->bounds.y);
              svg_info->bounds.x=0;
              svg_info->bounds.y=0;
              (void) FormatLocaleFile(svg_info->file,"rotate %g\n",angle);
              break;
            }
          if (LocaleCompare(keyword,"rx") == 0)
            {
              if (LocaleCompare((const char *) name,"ellipse") == 0)
                svg_info->element.major=
                  GetUserSpaceCoordinateValue(svg_info,1,value);
              else
                svg_info->radius.x=
                  GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"ry") == 0)
            {
              if (LocaleCompare((const char *) name,"ellipse") == 0)
                svg_info->element.minor=
                  GetUserSpaceCoordinateValue(svg_info,-1,value);
              else
                svg_info->radius.y=
                  GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (LocaleCompare(keyword,"stop-color") == 0)
            {
              (void) CloneString(&svg_info->stop_color,value);
              break;
            }
          if (LocaleCompare(keyword,"stroke") == 0)
            {
              if (LocaleCompare(value,"currentColor") == 0)
                {
                  (void) FormatLocaleFile(svg_info->file,"stroke \"%s\"\n",
                    color);
                  break;
                }
              (void) FormatLocaleFile(svg_info->file,"stroke \"%s\"\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-antialias %d\n",
                LocaleCompare(value,"true") == 0);
              break;
            }
          if (LocaleCompare(keyword,"stroke-dasharray") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-dasharray %s\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-dashoffset %g\n",
                GetUserSpaceCoordinateValue(svg_info,1,value));
              break;
            }
          if (LocaleCompare(keyword,"stroke-linecap") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-linecap \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-linejoin") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-linejoin \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,
                "stroke-miterlimit \"%s\"\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-opacity") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-opacity \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-width") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"stroke-width %g\n",
                GetUserSpaceCoordinateValue(svg_info,1,value));
              break;
            }
          if (LocaleCompare(keyword,"style") == 0)
            {
              SVGProcessStyleElement(context,name,value);
              break;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (LocaleCompare(keyword,"text-align") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"text-align \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"text-anchor") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"text-anchor \"%s\"\n",
                value);
              break;
            }
          if (LocaleCompare(keyword,"text-decoration") == 0)
            {
              if (LocaleCompare(value,"underline") == 0)
                (void) FormatLocaleFile(svg_info->file,"decorate underline\n");
              if (LocaleCompare(value,"line-through") == 0)
                (void) FormatLocaleFile(svg_info->file,
                  "decorate line-through\n");
              if (LocaleCompare(value,"overline") == 0)
                (void) FormatLocaleFile(svg_info->file,"decorate overline\n");
              break;
            }
          if (LocaleCompare(keyword,"text-antialiasing") == 0)
            {
              (void) FormatLocaleFile(svg_info->file,"text-antialias %d\n",
                LocaleCompare(value,"true") == 0);
              break;
            }
          if (LocaleCompare(keyword,"transform") == 0)
            {
              AffineMatrix
                affine,
                current,
                transform;

              GetAffineMatrix(&transform);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  ");
              tokens=SVGKeyValuePairs(context,'(',')',value,&number_tokens);
              if (tokens == (char **) NULL)
                break;
              for (j=0; j < (ssize_t) (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    %s: %s",keyword,value);
                current=transform;
                GetAffineMatrix(&affine);
                switch (*keyword)
                {
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare(keyword,"matrix") == 0)
                      {
                        p=(const char *) value;
                        GetNextToken(p,&p,MaxTextExtent,token);
                        affine.sx=StringToDouble(value,(char **) NULL);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.rx=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.ry=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.sy=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.tx=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MaxTextExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MaxTextExtent,token);
                        affine.ty=StringToDouble(token,&next_token);
                        break;
                      }
                    break;
                  }
                  case 'R':
                  case 'r':
                  {
                    if (LocaleCompare(keyword,"rotate") == 0)
                      {
                        double
                          angle,
                          x,
                          y;

                        p=(const char *) value;
                        GetNextToken(p,&p,MagickPathExtent,token);
                        angle=StringToDouble(value,(char **) NULL);
                        affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                        affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                        affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                        affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                        GetNextToken(p,&p,MagickPathExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MagickPathExtent,token);
                        x=StringToDouble(token,&next_token);
                        GetNextToken(p,&p,MagickPathExtent,token);
                        if (*token == ',')
                          GetNextToken(p,&p,MagickPathExtent,token);
                        y=StringToDouble(token,&next_token);
                        affine.tx=svg_info->bounds.x+x*
                          cos(DegreesToRadians(fmod(angle,360.0)))+y*
                          sin(DegreesToRadians(fmod(angle,360.0)));
                        affine.ty=svg_info->bounds.y-x*
                          sin(DegreesToRadians(fmod(angle,360.0)))+y*
                          cos(DegreesToRadians(fmod(angle,360.0)));
                        affine.tx-=x/2.0;
                        affine.ty-=y/2.0;
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
                    if (LocaleCompare(keyword,"scale") == 0)
                      {
                        for (p=(const char *) value; *p != '\0'; p++)
                          if ((isspace((int) ((unsigned char) *p)) != 0) ||
                              (*p == ','))
                            break;
                        affine.sx=GetUserSpaceCoordinateValue(svg_info,1,value);
                        affine.sy=affine.sx;
                        if (*p != '\0')
                          affine.sy=GetUserSpaceCoordinateValue(svg_info,-1,
                            p+1);
                        svg_info->scale[svg_info->n]=ExpandAffine(&affine);
                        break;
                      }
                    if (LocaleCompare(keyword,"skewX") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.ry=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,1,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    if (LocaleCompare(keyword,"skewY") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.rx=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,-1,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    break;
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare(keyword,"translate") == 0)
                      {
                        for (p=(const char *) value; *p != '\0'; p++)
                          if ((isspace((int) ((unsigned char) *p)) != 0) ||
                              (*p == ','))
                            break;
                        affine.tx=GetUserSpaceCoordinateValue(svg_info,1,value);
                        affine.ty=affine.tx;
                        if (*p != '\0')
                          affine.ty=GetUserSpaceCoordinateValue(svg_info,-1,
                            p+1);
                        break;
                      }
                    break;
                  }
                  default:
                    break;
                }
                transform.sx=affine.sx*current.sx+affine.ry*current.rx;
                transform.rx=affine.rx*current.sx+affine.sy*current.rx;
                transform.ry=affine.sx*current.ry+affine.ry*current.sy;
                transform.sy=affine.rx*current.ry+affine.sy*current.sy;
                transform.tx=affine.tx*current.sx+affine.ty*current.ry+
                  current.tx;
                transform.ty=affine.tx*current.rx+affine.ty*current.sy+
                  current.ty;
              }
              (void) FormatLocaleFile(svg_info->file,
                "affine %g %g %g %g %g %g\n",transform.sx,transform.rx,
                transform.ry,transform.sy,transform.tx,transform.ty);
              for (j=0; tokens[j] != (char *) NULL; j++)
                tokens[j]=DestroyString(tokens[j]);
              tokens=(char **) RelinquishMagickMemory(tokens);
              break;
            }
          break;
        }
        case 'V':
        case 'v':
        {
          if (LocaleCompare(keyword,"verts") == 0)
            {
              (void) CloneString(&svg_info->vertices,value);
              break;
            }
          if (LocaleCompare(keyword,"viewBox") == 0)
            {
              p=(const char *) value;
              GetNextToken(p,&p,MaxTextExtent,token);
              svg_info->view_box.x=StringToDouble(token,&next_token);
              GetNextToken(p,&p,MaxTextExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MaxTextExtent,token);
              svg_info->view_box.y=StringToDouble(token,&next_token);
              GetNextToken(p,&p,MaxTextExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MaxTextExtent,token);
              svg_info->view_box.width=StringToDouble(token,
                (char **) NULL);
              if (svg_info->bounds.width == 0)
                svg_info->bounds.width=svg_info->view_box.width;
              GetNextToken(p,&p,MaxTextExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MaxTextExtent,token);
              svg_info->view_box.height=StringToDouble(token,
                (char **) NULL);
              if (svg_info->bounds.height == 0)
                svg_info->bounds.height=svg_info->view_box.height;
              break;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (LocaleCompare(keyword,"width") == 0)
            {
              svg_info->bounds.width=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          break;
        }
        case 'X':
        case 'x':
        {
          if (LocaleCompare(keyword,"x") == 0)
            {
              svg_info->bounds.x=GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"xlink:href") == 0)
            {
              (void) CloneString(&svg_info->url,value);
              break;
            }
          if (LocaleCompare(keyword,"x1") == 0)
            {
              svg_info->segment.x1=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          if (LocaleCompare(keyword,"x2") == 0)
            {
              svg_info->segment.x2=
                GetUserSpaceCoordinateValue(svg_info,1,value);
              break;
            }
          break;
        }
        case 'Y':
        case 'y':
        {
          if (LocaleCompare(keyword,"y") == 0)
            {
              svg_info->bounds.y=GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          if (LocaleCompare(keyword,"y1") == 0)
            {
              svg_info->segment.y1=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          if (LocaleCompare(keyword,"y2") == 0)
            {
              svg_info->segment.y2=
                GetUserSpaceCoordinateValue(svg_info,-1,value);
              break;
            }
          break;
        }
        default:
          break;
      }
    }
  if (LocaleCompare((const char *) name,"svg") == 0)
    {
      if (svg_info->document->encoding != (const xmlChar *) NULL)
        (void) FormatLocaleFile(svg_info->file,"encoding \"%s\"\n",
          (const char *) svg_info->document->encoding);
      if (attributes != (const xmlChar **) NULL)
        {
          double
            sx,
            sy,
            tx,
            ty;

          if ((svg_info->view_box.width == 0.0) ||
              (svg_info->view_box.height == 0.0))
            svg_info->view_box=svg_info->bounds;
          svg_info->width=0;
          if (svg_info->bounds.width > 0.0)
            svg_info->width=(size_t) floor(svg_info->bounds.width+0.5);
          svg_info->height=0;
          if (svg_info->bounds.height > 0.0)
            svg_info->height=(size_t) floor(svg_info->bounds.height+0.5);
          (void) FormatLocaleFile(svg_info->file,"viewbox 0 0 %.20g %.20g\n",
            (double) svg_info->width,(double) svg_info->height);
          sx=(double) svg_info->width/svg_info->view_box.width;
          sy=(double) svg_info->height/svg_info->view_box.height;
          tx=svg_info->view_box.x != 0.0 ? (double) -sx*svg_info->view_box.x :
            0.0;
          ty=svg_info->view_box.y != 0.0 ? (double) -sy*svg_info->view_box.y :
            0.0;
          (void) FormatLocaleFile(svg_info->file,"affine %g 0 0 %g %g %g\n",
            sx,sy,tx,ty);
          if ((svg_info->svgDepth == 1) && (*background != '\0'))
            {
              PushGraphicContext(id);
              (void) FormatLocaleFile(svg_info->file,"fill %s\n",background);
              (void) FormatLocaleFile(svg_info->file,
                "rectangle 0,0 %g,%g\n",svg_info->view_box.width,
                svg_info->view_box.height);
              (void) FormatLocaleFile(svg_info->file,"pop graphic-context\n");
            }
        }
    }
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  )");
  if (units != (char *) NULL)
    units=DestroyString(units);
  if (color != (char *) NULL)
    color=DestroyString(color);
}