Perl_pregcomp(pTHX_ SV * const pattern, const U32 flags)
{
    regexp_engine const *eng = current_re_engine();
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_PREGCOMP;

    /* Dispatch a request to compile a regexp to correct regexp engine. */
    DEBUG_COMPILE_r({
        Perl_re_printf( aTHX_  "Using engine %" UVxf "\n",
			PTR2UV(eng));
    });
    return CALLREGCOMP_ENG(eng, pattern, flags);
}