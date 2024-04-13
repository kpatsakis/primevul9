endTimer(const char *fmt, ...)
{
    long msec;
    va_list ap;

    end = clock();
    msec = ((end - begin) * 1000) / CLOCKS_PER_SEC;

#ifndef HAVE_STDARG_H
#error "endTimer required stdarg functions"
#endif
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, " took %ld ms\n", msec);
}