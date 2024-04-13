static inline char *_validate_env(const char *env)
{
    char *res, *c;

    if (env)
        res = g_strdup(env);
    else
        res = g_strdup("");
    for (c = res; *c; c++)
        if (*c == '\n' || *c == '\t')
            *c = ' ';
    return res;
}