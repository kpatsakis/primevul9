dirent_match_brace(const char *pattern, VALUE val, void *enc)
{
    struct dirent_brace_args *arg = (struct dirent_brace_args *)val;

    return dirent_match(pattern, enc, arg->name, arg->dp, arg->flags);
}