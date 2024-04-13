static int getmailboxidexists(void *sc, const char *extname)
{
    script_data_t *sd = (script_data_t *)sc;
    const char *userid = mbname_userid(sd->mbname);
    char *intname = mboxlist_find_uniqueid(extname, userid, sd->authstate);
    int exists = intname ? 1 : 0;
    free(intname);
    return exists;
}