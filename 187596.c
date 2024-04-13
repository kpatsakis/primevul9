EXPORTED int etagcmp(const char *hdr, const char *etag)
{
    size_t len;

    if (!etag) return -1;               /* no representation       */
    if (!strcmp(hdr, "*")) return 0;    /* any representation      */

    len = strlen(etag);
    if (!strncmp(hdr, "W/", 2)) hdr+=2; /* skip weak prefix        */
    if (*hdr++ != '\"') return 1;       /* match/skip open DQUOTE  */
    if (strlen(hdr) != len+1) return 1; /* make sure lengths match */
    if (hdr[len] != '\"') return 1;     /* match close DQUOTE      */

    return strncmp(hdr, etag, len);
}