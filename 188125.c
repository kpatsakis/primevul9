static char *TracePSClippath(const unsigned char *blob,size_t length)
{
  char
    *path,
    *message;

  MagickBooleanType
    in_subpath;

  PointInfo
    first[3],
    last[3],
    point[3];

  register ssize_t
    i,
    x;

  ssize_t
    knot_count,
    selector,
    y;

  path=AcquireString((char *) NULL);
  if (path == (char *) NULL)
    return((char *) NULL);
  message=AcquireString((char *) NULL);
  (void) FormatLocaleString(message,MagickPathExtent,"/ClipImage\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,"{\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,
    "  /c {curveto} bind def\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,
    "  /l {lineto} bind def\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,
    "  /m {moveto} bind def\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,
    "  /v {currentpoint 6 2 roll curveto} bind def\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,
    "  /y {2 copy curveto} bind def\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,
    "  /z {closepath} bind def\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,"  newpath\n");
  (void) ConcatenateString(&path,message);
  /*
    The clipping path format is defined in "Adobe Photoshop File Formats
    Specification" version 6.0 downloadable from adobe.com.
  */
  (void) memset(point,0,sizeof(point));
  (void) memset(first,0,sizeof(first));
  (void) memset(last,0,sizeof(last));
  knot_count=0;
  in_subpath=MagickFalse;
  while (length > 0)
  {
    selector=(ssize_t) ReadPropertyMSBShort(&blob,&length);
    switch (selector)
    {
      case 0:
      case 3:
      {
        if (knot_count != 0)
          {
            blob+=24;
            length-=MagickMin(24,(ssize_t) length);
            break;
          }
        /*
          Expected subpath length record.
        */
        knot_count=(ssize_t) ReadPropertyMSBShort(&blob,&length);
        blob+=22;
        length-=MagickMin(22,(ssize_t) length);
        break;
      }
      case 1:
      case 2:
      case 4:
      case 5:
      {
        if (knot_count == 0)
          {
            /*
              Unexpected subpath knot
            */
            blob+=24;
            length-=MagickMin(24,(ssize_t) length);
            break;
          }
        /*
          Add sub-path knot
        */
        for (i=0; i < 3; i++)
        {
          size_t
            xx,
            yy;

          yy=(size_t) ReadPropertyMSBLong(&blob,&length);
          xx=(size_t) ReadPropertyMSBLong(&blob,&length);
          x=(ssize_t) xx;
          if (xx > 2147483647)
            x=(ssize_t) xx-4294967295U-1;
          y=(ssize_t) yy;
          if (yy > 2147483647)
            y=(ssize_t) yy-4294967295U-1;
          point[i].x=(double) x/4096/4096;
          point[i].y=1.0-(double) y/4096/4096;
        }
        if (in_subpath == MagickFalse)
          {
            (void) FormatLocaleString(message,MagickPathExtent,"  %g %g m\n",
              point[1].x,point[1].y);
            for (i=0; i < 3; i++)
            {
              first[i]=point[i];
              last[i]=point[i];
            }
          }
        else
          {
            /*
              Handle special cases when Bezier curves are used to describe
              corners and straight lines.
            */
            if ((last[1].x == last[2].x) && (last[1].y == last[2].y) &&
                (point[0].x == point[1].x) && (point[0].y == point[1].y))
              (void) FormatLocaleString(message,MagickPathExtent,
                "  %g %g l\n",point[1].x,point[1].y);
            else
              if ((last[1].x == last[2].x) && (last[1].y == last[2].y))
                (void) FormatLocaleString(message,MagickPathExtent,
                  "  %g %g %g %g v\n",point[0].x,point[0].y,
                  point[1].x,point[1].y);
              else
                if ((point[0].x == point[1].x) && (point[0].y == point[1].y))
                  (void) FormatLocaleString(message,MagickPathExtent,
                    "  %g %g %g %g y\n",last[2].x,last[2].y,
                    point[1].x,point[1].y);
                else
                  (void) FormatLocaleString(message,MagickPathExtent,
                    "  %g %g %g %g %g %g c\n",last[2].x,
                    last[2].y,point[0].x,point[0].y,point[1].x,point[1].y);
            for (i=0; i < 3; i++)
              last[i]=point[i];
          }
        (void) ConcatenateString(&path,message);
        in_subpath=MagickTrue;
        knot_count--;
        /*
          Close the subpath if there are no more knots.
        */
        if (knot_count == 0)
          {
            /*
              Same special handling as above except we compare to the
              first point in the path and close the path.
            */
            if ((last[1].x == last[2].x) && (last[1].y == last[2].y) &&
                (first[0].x == first[1].x) && (first[0].y == first[1].y))
              (void) FormatLocaleString(message,MagickPathExtent,
                "  %g %g l z\n",first[1].x,first[1].y);
            else
              if ((last[1].x == last[2].x) && (last[1].y == last[2].y))
                (void) FormatLocaleString(message,MagickPathExtent,
                  "  %g %g %g %g v z\n",first[0].x,first[0].y,
                  first[1].x,first[1].y);
              else
                if ((first[0].x == first[1].x) && (first[0].y == first[1].y))
                  (void) FormatLocaleString(message,MagickPathExtent,
                    "  %g %g %g %g y z\n",last[2].x,last[2].y,
                    first[1].x,first[1].y);
                else
                  (void) FormatLocaleString(message,MagickPathExtent,
                    "  %g %g %g %g %g %g c z\n",last[2].x,
                    last[2].y,first[0].x,first[0].y,first[1].x,first[1].y);
            (void) ConcatenateString(&path,message);
            in_subpath=MagickFalse;
          }
        break;
      }
      case 6:
      case 7:
      case 8:
      default:
      {
        blob+=24;
        length-=MagickMin(24,(ssize_t) length);
        break;
      }
    }
  }
  /*
    Returns an empty PS path if the path has no knots.
  */
  (void) FormatLocaleString(message,MagickPathExtent,"  eoclip\n");
  (void) ConcatenateString(&path,message);
  (void) FormatLocaleString(message,MagickPathExtent,"} bind def");
  (void) ConcatenateString(&path,message);
  message=DestroyString(message);
  return(path);
}