png_get_copyright(png_const_structrp png_ptr)
{
   PNG_UNUSED(png_ptr)  /* Silence compiler warning about unused png_ptr */
#ifdef PNG_STRING_COPYRIGHT
   return PNG_STRING_COPYRIGHT
#else
#  ifdef __STDC__
   return PNG_STRING_NEWLINE \
      "libpng version 1.6.27 - December 29, 2016" PNG_STRING_NEWLINE \
      "Copyright (c) 1998-2002,2004,2006-2016 Glenn Randers-Pehrson" \
      PNG_STRING_NEWLINE \
      "Copyright (c) 1996-1997 Andreas Dilger" PNG_STRING_NEWLINE \
      "Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc." \
      PNG_STRING_NEWLINE;
#  else
   return "libpng version 1.6.27 - December 29, 2016\
      Copyright (c) 1998-2002,2004,2006-2016 Glenn Randers-Pehrson\
      Copyright (c) 1996-1997 Andreas Dilger\
      Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc.";
#  endif
#endif
}