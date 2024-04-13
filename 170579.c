static int getmetadata(void *sc, const char *extname, const char *keyname, char **res)
{
    script_data_t *sd = (script_data_t *)sc;
    struct buf attrib = BUF_INITIALIZER;
    char *intname = !extname ? xstrdup("") :
        mboxname_from_externalUTF8(extname, sd->ns, mbname_userid(sd->mbname));
    int r;
    if (!strncmp(keyname, "/private/", 9)) {
        r = annotatemore_lookup(intname, keyname+8, mbname_userid(sd->mbname), &attrib);
    }
    else if (!strncmp(keyname, "/shared/", 8)) {
        r = annotatemore_lookup(intname, keyname+7, "", &attrib);
    }
    else {
        r = IMAP_MAILBOX_NONEXISTENT;
    }
    *res = (r || !attrib.len) ? NULL : buf_release(&attrib);
    free(intname);
    buf_free(&attrib);
    return r ? 0 : 1;
}