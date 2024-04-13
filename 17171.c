pstrcmp(const void *a, const void *b)
{
    return (pathcmp(*(char **)a, *(char **)b, -1));
}