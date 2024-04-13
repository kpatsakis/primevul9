skip_regexp(
    char_u	*startp,
    int		delim,
    int		magic)
{
    return skip_regexp_ex(startp, delim, magic, NULL, NULL, NULL);
}