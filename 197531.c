MagickBooleanType ListThresholdMapFile(FILE *file,const char *xml,
  const char *filename,ExceptionInfo *exception)
{
  XMLTreeInfo *thresholds,*threshold,*description;
  const char *map,*alias,*content;

  assert( xml != (char *) NULL );
  assert( file != (FILE *) NULL );

  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Loading threshold map file \"%s\" ...",filename);
  thresholds=NewXMLTree(xml,exception);
  if ( thresholds == (XMLTreeInfo *) NULL )
    return(MagickFalse);

  (void) FormatLocaleFile(file,"%-16s %-12s %s\n","Map","Alias","Description");
  (void) FormatLocaleFile(file,
    "----------------------------------------------------\n");

  for( threshold = GetXMLTreeChild(thresholds,"threshold");
       threshold != (XMLTreeInfo *) NULL;
       threshold = GetNextXMLTreeTag(threshold) )
  {
    map = GetXMLTreeAttribute(threshold, "map");
    if (map == (char *) NULL) {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingAttribute", "<map>");
      thresholds=DestroyXMLTree(thresholds);
      return(MagickFalse);
    }
    alias = GetXMLTreeAttribute(threshold, "alias");
    /* alias is optional, no if test needed */
    description=GetXMLTreeChild(threshold,"description");
    if ( description == (XMLTreeInfo *) NULL ) {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingElement", "<description>, map \"%s\"", map);
      thresholds=DestroyXMLTree(thresholds);
      return(MagickFalse);
    }
    content=GetXMLTreeContent(description);
    if ( content == (char *) NULL ) {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "XmlMissingContent", "<description>, map \"%s\"", map);
      thresholds=DestroyXMLTree(thresholds);
      return(MagickFalse);
    }
    (void) FormatLocaleFile(file,"%-16s %-12s %s\n",map,alias ? alias : "",
      content);
  }
  thresholds=DestroyXMLTree(thresholds);
  return(MagickTrue);
}