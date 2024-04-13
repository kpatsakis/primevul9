png_get_header_version(png_const_structrp png_ptr)
{
   /* Returns longer string containing both version and date */
   PNG_UNUSED(png_ptr)  /* Silence compiler warning about unused png_ptr */
#ifdef __STDC__
   return PNG_HEADER_VERSION_STRING
#  ifndef PNG_READ_SUPPORTED
      " (NO READ SUPPORT)"
#  endif
      PNG_STRING_NEWLINE;
#else
   return PNG_HEADER_VERSION_STRING;
#endif
}