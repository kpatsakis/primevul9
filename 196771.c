static char* _zzip_restrict _zzip_strndup(char* p, int maxlen)
{
    if (! p) return 0;
    ___ char* r = malloc (maxlen+1);
    if (! r) return r;
    strncpy (r, p, maxlen);
    r[maxlen] = '\0';
    return r; ____;
}