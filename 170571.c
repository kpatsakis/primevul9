static int getmailboxexists(void *sc, const char *extname)
{
    script_data_t *sd = (script_data_t *)sc;
    char *intname = mboxname_from_externalUTF8(extname, sd->ns,
                                               mbname_userid(sd->mbname));
    int r = mboxlist_lookup(intname, NULL, NULL);
    free(intname);
    return r ? 0 : 1; /* 0 => exists */
}