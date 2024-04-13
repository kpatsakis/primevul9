static char *mboxname_from_externalUTF8(const char *extname,
                                        const struct namespace *ns,
                                        const char *userid)
{
    char *intname, *freeme = NULL;

    if (config_getswitch(IMAPOPT_SIEVE_UTF8FILEINTO)) {
        freeme = xmalloc(5 * strlen(extname) + 1);
        UTF8_to_mUTF7(freeme, extname);
        extname = freeme;
    }

    intname = mboxname_from_external(extname, ns, userid);
    free(freeme);

    return intname;
}