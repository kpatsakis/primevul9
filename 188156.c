static const char *GetMagickPropertyLetter(ImageInfo *image_info,
  Image *image,const char letter,ExceptionInfo *exception)
{
#define WarnNoImageReturn(format,arg) \
  if (image == (Image *) NULL ) { \
    (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning, \
      "NoImageForProperty",format,arg); \
    return((const char *) NULL); \
  }
#define WarnNoImageInfoReturn(format,arg) \
  if (image_info == (ImageInfo *) NULL ) { \
    (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning, \
      "NoImageInfoForProperty",format,arg); \
    return((const char *) NULL); \
  }

  char
    value[MagickPathExtent];  /* formatted string to store as an artifact */

  const char
    *string;     /* return a string already stored somewher */

  if ((image != (Image *) NULL) && (image->debug != MagickFalse))
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  else
    if ((image_info != (ImageInfo *) NULL) &&
        (image_info->debug != MagickFalse))
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s","no-images");
  *value='\0';           /* formatted string */
  string=(char *) NULL;  /* constant string reference */
  /*
    Get properities that are directly defined by images.
  */
  switch (letter)
  {
    case 'b':  /* image size read in - in bytes */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatMagickSize(image->extent,MagickFalse,"B",MagickPathExtent,
        value);
      if (image->extent == 0)
        (void) FormatMagickSize(GetBlobSize(image),MagickFalse,"B",
          MagickPathExtent,value);
      break;
    }
    case 'c':  /* image comment property - empty string by default */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=GetImageProperty(image,"comment",exception);
      if ( string == (const char *) NULL )
        string="";
      break;
    }
    case 'd':  /* Directory component of filename */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      GetPathComponent(image->magick_filename,HeadPath,value);
      if (*value == '\0')
        string="";
      break;
    }
    case 'e': /* Filename extension (suffix) of image file */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      GetPathComponent(image->magick_filename,ExtensionPath,value);
      if (*value == '\0')
        string="";
      break;
    }
    case 'f': /* Filename without directory component */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      GetPathComponent(image->magick_filename,TailPath,value);
      if (*value == '\0')
        string="";
      break;
    }
    case 'g': /* Image geometry, canvas and offset  %Wx%H+%X+%Y */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,
        "%.20gx%.20g%+.20g%+.20g",(double) image->page.width,(double)
        image->page.height,(double) image->page.x,(double) image->page.y);
      break;
    }
    case 'h': /* Image height (current) */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        (image->rows != 0 ? image->rows : image->magick_rows));
      break;
    }
    case 'i': /* Filename last used for an image (read or write) */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=image->filename;
      break;
    }
    case 'k': /* Number of unique colors  */
    {
      /*
        FUTURE: ensure this does not generate the formatted comment!
      */
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        GetNumberColors(image,(FILE *) NULL,exception));
      break;
    }
    case 'l': /* Image label property - empty string by default */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=GetImageProperty(image,"label",exception);
      if (string == (const char *) NULL)
        string="";
      break;
    }
    case 'm': /* Image format (file magick) */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=image->magick;
      break;
    }
    case 'n': /* Number of images in the list.  */
    {
      if ( image != (Image *) NULL )
        (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
          GetImageListLength(image));
      else
        string="0";    /* no images or scenes */
      break;
    }
    case 'o': /* Output Filename - for delegate use only */
      WarnNoImageInfoReturn("\"%%%c\"",letter);
      string=image_info->filename;
      break;
    case 'p': /* Image index in current image list */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        GetImageIndexInList(image));
      break;
    }
    case 'q': /* Quantum depth of image in memory */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        MAGICKCORE_QUANTUM_DEPTH);
      break;
    }
    case 'r': /* Image storage class, colorspace, and alpha enabled.  */
    {
      ColorspaceType
        colorspace;

      WarnNoImageReturn("\"%%%c\"",letter);
      colorspace=image->colorspace;
      if ((image->columns != 0) && (image->rows != 0) &&
          (SetImageGray(image,exception) != MagickFalse))
        colorspace=GRAYColorspace;   /* FUTURE: this is IMv6 not IMv7 */
      (void) FormatLocaleString(value,MagickPathExtent,"%s %s %s",
        CommandOptionToMnemonic(MagickClassOptions,(ssize_t)
        image->storage_class),CommandOptionToMnemonic(MagickColorspaceOptions,
        (ssize_t) colorspace),image->alpha_trait != UndefinedPixelTrait ?
        "Alpha" : "");
      break;
    }
    case 's': /* Image scene number */
    {
#if 0  /* this seems non-sensical -- simplifing */
      if (image_info->number_scenes != 0)
        (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
          image_info->scene);
      else if (image != (Image *) NULL)
        (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
          image->scene);
      else
          string="0";
#else
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        image->scene);
#endif
      break;
    }
    case 't': /* Base filename without directory or extention */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      GetPathComponent(image->magick_filename,BasePath,value);
      if (*value == '\0')
        string="";
      break;
    }
    case 'u': /* Unique filename */
    {
      WarnNoImageInfoReturn("\"%%%c\"",letter);
      string=image_info->unique;
      break;
    }
    case 'w': /* Image width (current) */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        (image->columns != 0 ? image->columns : image->magick_columns));
      break;
    }
    case 'x': /* Image horizontal resolution (with units) */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        fabs(image->resolution.x) > MagickEpsilon ? image->resolution.x : 72.0);
      break;
    }
    case 'y': /* Image vertical resolution (with units) */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",
        fabs(image->resolution.y) > MagickEpsilon ? image->resolution.y : 72.0);
      break;
    }
    case 'z': /* Image depth as read in */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        image->depth);
      break;
    }
    case 'A': /* Image alpha channel  */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=CommandOptionToMnemonic(MagickPixelTraitOptions,(ssize_t)
        image->alpha_trait);
      break;
    }
    case 'B':  /* image size read in - in bytes */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        image->extent);
      if (image->extent == 0)
        (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
          GetBlobSize(image));
      break;
    }
    case 'C': /* Image compression method.  */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=CommandOptionToMnemonic(MagickCompressOptions,(ssize_t)
        image->compression);
      break;
    }
    case 'D': /* Image dispose method.  */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=CommandOptionToMnemonic(MagickDisposeOptions,(ssize_t)
        image->dispose);
      break;
    }
    case 'G': /* Image size as geometry = "%wx%h" */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20gx%.20g",(double)
        image->magick_columns,(double) image->magick_rows);
      break;
    }
    case 'H': /* layer canvas height */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        image->page.height);
      break;
    }
    case 'M': /* Magick filename - filename given incl. coder & read mods */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=image->magick_filename;
      break;
    }
    case 'O': /* layer canvas offset with sign = "+%X+%Y" */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%+ld%+ld",(long)
        image->page.x,(long) image->page.y);
      break;
    }
    case 'P': /* layer canvas page size = "%Wx%H" */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20gx%.20g",(double)
        image->page.width,(double) image->page.height);
      break;
    }
    case 'Q': /* image compression quality */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        (image->quality == 0 ? 92 : image->quality));
      break;
    }
    case 'S': /* Number of scenes in image list.  */
    {
      WarnNoImageInfoReturn("\"%%%c\"",letter);
#if 0 /* What is this number? -- it makes no sense - simplifing */
      if (image_info->number_scenes == 0)
         string="2147483647";
      else if ( image != (Image *) NULL )
        (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
                image_info->scene+image_info->number_scenes);
      else
        string="0";
#else
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        (image_info->number_scenes == 0 ? 2147483647 :
         image_info->number_scenes));
#endif
      break;
    }
    case 'T': /* image time delay for animations */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        image->delay);
      break;
    }
    case 'U': /* Image resolution units. */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      string=CommandOptionToMnemonic(MagickResolutionOptions,(ssize_t)
        image->units);
      break;
    }
    case 'W': /* layer canvas width */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        image->page.width);
      break;
    }
    case 'X': /* layer canvas X offset */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%+.20g",(double)
        image->page.x);
      break;
    }
    case 'Y': /* layer canvas Y offset */
    {
      WarnNoImageReturn("\"%%%c\"",letter);
      (void) FormatLocaleString(value,MagickPathExtent,"%+.20g",(double)
        image->page.y);
      break;
    }
    case '%': /* percent escaped */
    {
      string="%";
      break;
    }
    case '@': /* Trim bounding box, without actually Trimming! */
    {
      RectangleInfo
        page;

      WarnNoImageReturn("\"%%%c\"",letter);
      page=GetImageBoundingBox(image,exception);
      (void) FormatLocaleString(value,MagickPathExtent,
        "%.20gx%.20g%+.20g%+.20g",(double) page.width,(double) page.height,
        (double) page.x,(double)page.y);
      break;
    }
    case '#':
    {
      /*
        Image signature.
      */
      WarnNoImageReturn("\"%%%c\"",letter);
      if ((image->columns != 0) && (image->rows != 0))
        (void) SignatureImage(image,exception);
      string=GetImageProperty(image,"signature",exception);
      break;
    }
  }
  if (string != (char *) NULL)
    return(string);
  if (*value != '\0')
    {
      /*
        Create a cloned copy of result.
      */
      if (image != (Image *) NULL)
        {
          (void) SetImageArtifact(image,"get-property",value);
          return(GetImageArtifact(image,"get-property"));
        }
      else
        {
          (void) SetImageOption(image_info,"get-property",value);
          return(GetImageOption(image_info,"get-property"));
        }
    }
  return((char *) NULL);
}