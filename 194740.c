string_regcomp (void *preg, const char *regex, int default_flags)
{
    const char *ptr_regex;
    int flags;

    ptr_regex = string_regex_flags (regex, default_flags, &flags);
    return regcomp ((regex_t *)preg, ptr_regex, flags);
}