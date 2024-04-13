int _bson_errprintf( const char *format, ... ) {
    va_list ap;
    int ret = 0;
    va_start( ap, format );
#ifndef R_SAFETY_NET
    ret = vfprintf( stderr, format, ap );
#endif
    va_end( ap );

    return ret;
}