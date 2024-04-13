Perl_re_printf(pTHX_ const char *fmt, ...)
{
    va_list ap;
    int result;
    PerlIO *f= Perl_debug_log;
    PERL_ARGS_ASSERT_RE_PRINTF;
    va_start(ap, fmt);
    result = PerlIO_vprintf(f, fmt, ap);
    va_end(ap);
    return result;
}