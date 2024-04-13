MagickExport ThresholdMap *GetThresholdMapFile(const char *xml,
  const char *filename,const char *map_id,ExceptionInfo *exception)
{
  const char
    *attribute,
    *content;

  double
    value;

  ThresholdMap
     *map;

  XMLTreeInfo
     *description,
     *levels,
     *threshold,
     *thresholds;

  map = (ThresholdMap *) NULL;
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Loading threshold map file \"%s\" ...",filename);
  thresholds=NewXMLTree(xml,exception);
  if ( thresholds == (XMLTreeInfo *) NULL )
    return(map);
  for (threshold = GetXMLTreeChild(thresholds,"threshold");
       threshold != (XMLTreeInfo *) NULL;
       threshold = GetNextXMLTreeTag(threshold) )
  {
    attribute=GetXMLTreeAttribute(threshold, "map");
    if ((attribute != (char *) NULL) && (LocaleCompare(map_id,attribute) == 0))
      break;
    attribute=GetXMLTreeAttribute(threshold, "alias");
    if ((attribute != (char *) NULL) && (LocaleCompare(map_id,attribute) == 0))
      break;
  }
  if (threshold == (XMLTreeInfo *) NULL)
    {
      thresholds=DestroyXMLTree(thresholds);
      return(map);
    }
  description=GetXMLTreeChild(threshold,"description");
  if (description == (XMLTreeInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingElement", "<description>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      return(map);
    }
  levels=GetXMLTreeChild(threshold,"levels");
  if (levels == (XMLTreeInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingElement", "<levels>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      return(map);
    }
  /*
    The map has been found -- allocate a Threshold Map to return
  */
  map=(ThresholdMap *) AcquireMagickMemory(sizeof(ThresholdMap));
  if (map == (ThresholdMap *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireThresholdMap");
  map->map_id=(char *) NULL;
  map->description=(char *) NULL;
  map->levels=(ssize_t *) NULL;
  /*
    Assign basic attributeibutes.
  */
  attribute=GetXMLTreeAttribute(threshold,"map");
  if (attribute != (char *) NULL)
    map->map_id=ConstantString(attribute);
  content=GetXMLTreeContent(description);
  if (content != (char *) NULL)
    map->description=ConstantString(content);
  attribute=GetXMLTreeAttribute(levels,"width");
  if (attribute == (char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingAttribute", "<levels width>, map \"%s\"",map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  map->width=StringToUnsignedLong(attribute);
  if (map->width == 0)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
       "XmlInvalidAttribute", "<levels width>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  attribute=GetXMLTreeAttribute(levels,"height");
  if (attribute == (char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingAttribute", "<levels height>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  map->height=StringToUnsignedLong(attribute);
  if (map->height == 0)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlInvalidAttribute", "<levels height>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  attribute=GetXMLTreeAttribute(levels, "divisor");
  if (attribute == (char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingAttribute", "<levels divisor>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  map->divisor=(ssize_t) StringToLong(attribute);
  if (map->divisor < 2)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlInvalidAttribute", "<levels divisor>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  /*
    Allocate theshold levels array.
  */
  content=GetXMLTreeContent(levels);
  if (content == (char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingContent", "<levels>, map \"%s\"", map_id);
      thresholds=DestroyXMLTree(thresholds);
      map=DestroyThresholdMap(map);
      return(map);
    }
  map->levels=(ssize_t *) AcquireQuantumMemory((size_t) map->width,map->height*
    sizeof(*map->levels));
  if (map->levels == (ssize_t *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireThresholdMap");
  {
    char
      *p;

    register ssize_t
      i;

    /*
      Parse levels into integer array.
    */
    for (i=0; i< (ssize_t) (map->width*map->height); i++)
    {
      map->levels[i]=(ssize_t) strtol(content,&p,10);
      if (p == content)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "XmlInvalidContent", "<level> too few values, map \"%s\"", map_id);
          thresholds=DestroyXMLTree(thresholds);
          map=DestroyThresholdMap(map);
          return(map);
        }
      if ((map->levels[i] < 0) || (map->levels[i] > map->divisor))
        {
          (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "XmlInvalidContent", "<level> %.20g out of range, map \"%s\"",
            (double) map->levels[i],map_id);
          thresholds=DestroyXMLTree(thresholds);
          map=DestroyThresholdMap(map);
          return(map);
        }
      content=p;
    }
    value=(double) strtol(content,&p,10);
    (void) value;
    if (p != content)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
          "XmlInvalidContent", "<level> too many values, map \"%s\"", map_id);
       thresholds=DestroyXMLTree(thresholds);
       map=DestroyThresholdMap(map);
       return(map);
     }
  }
  thresholds=DestroyXMLTree(thresholds);
  return(map);
}