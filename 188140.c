static MagickBooleanType ValidateXMPProfile(const char *profile,
  const size_t length)
{
#if defined(MAGICKCORE_XML_DELEGATE)
  {
    xmlDocPtr
      document;

    /*
      Parse XML profile.
    */
    document=xmlReadMemory(profile,(int) length,"xmp.xml",NULL,
      XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (document == (xmlDocPtr) NULL)
      return(MagickFalse);
    xmlFreeDoc(document);
    return(MagickTrue);
  }
#else
  return(MagickFalse);
#endif
}