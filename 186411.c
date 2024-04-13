static void MSLEndElement(void *context,const xmlChar *tag)
{
  ssize_t
    n;

  MSLInfo
    *msl_info;

  /*
    Called when the end of an element has been detected.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.endElement(%s)",
    tag);
  msl_info=(MSLInfo *) context;
  n=msl_info->n;
  switch (*tag)
  {
    case 'C':
    case 'c':
    {
      if (LocaleCompare((const char *) tag,"comment") == 0 )
        {
          (void) DeleteImageProperty(msl_info->image[n],"comment");
          if (msl_info->content == (char *) NULL)
            break;
          StripString(msl_info->content);
          (void) SetImageProperty(msl_info->image[n],"comment",
            msl_info->content);
          break;
        }
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare((const char *) tag, "group") == 0 )
      {
        if (msl_info->group_info[msl_info->number_groups-1].numImages > 0 )
        {
          ssize_t  i = (ssize_t)
            (msl_info->group_info[msl_info->number_groups-1].numImages);
          while ( i-- )
          {
            if (msl_info->image[msl_info->n] != (Image *) NULL)
              msl_info->image[msl_info->n]=DestroyImage(msl_info->image[msl_info->n]);
            msl_info->attributes[msl_info->n]=DestroyImage(msl_info->attributes[msl_info->n]);
            msl_info->image_info[msl_info->n]=DestroyImageInfo(msl_info->image_info[msl_info->n]);
            msl_info->n--;
          }
        }
        msl_info->number_groups--;
      }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((const char *) tag, "image") == 0)
        MSLPopImage(msl_info);
       break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare((const char *) tag,"label") == 0 )
        {
          (void) DeleteImageProperty(msl_info->image[n],"label");
          if (msl_info->content == (char *) NULL)
            break;
          StripString(msl_info->content);
          (void) SetImageProperty(msl_info->image[n],"label",
            msl_info->content);
          break;
        }
      break;
    }
    case 'M':
    case 'm':
    {
      if (LocaleCompare((const char *) tag, "msl") == 0 )
      {
        /*
          This our base element.
            at the moment we don't do anything special
            but someday we might!
        */
      }
      break;
    }
    default:
      break;
  }
  if (msl_info->content != (char *) NULL)
    msl_info->content=DestroyString(msl_info->content);
}