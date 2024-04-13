void helpf(FILE *errors, const char *fmt, ...)
{
  va_list ap;
  if(fmt) {
    va_start(ap, fmt);
    fputs("curl: ", errors); /* prefix it */
    vfprintf(errors, fmt, ap);
    va_end(ap);
  }
  fprintf(errors, "curl: try 'curl --help' "
#ifdef USE_MANUAL
          "or 'curl --manual' "
#endif
          "for more information\n");
}