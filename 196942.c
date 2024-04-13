endTimer(char *format, ...)
{
    /*
     * We cannot do anything because we don't have a timing function
     */
#ifdef HAVE_STDARG_H
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, " was not timed\n");
#else
    /* We don't have gettimeofday, time or stdarg.h, what crazy world is
     * this ?!
     */
#endif
}