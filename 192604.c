Perl_re_indentf(pTHX_ const char *fmt, U32 depth, ...)
{
    va_list ap;
    int result;
    PerlIO *f= Perl_debug_log;
    PERL_ARGS_ASSERT_RE_INDENTF;
    va_start(ap, depth);
    PerlIO_printf(f, "%*s", ( (int)depth % 20 ) * 2, "");
    result = PerlIO_vprintf(f, fmt, ap);
    va_end(ap);
    return result;
}