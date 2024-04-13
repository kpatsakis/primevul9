static int getspecialuseexists(void *sc, const char *extname, strarray_t *uses)
{
    script_data_t *sd = (script_data_t *)sc;
    const char *userid = mbname_userid(sd->mbname);
    int i, r = 1;

    if (extname) {
        char *intname = mboxname_from_externalUTF8(extname, sd->ns, userid);
        struct buf attrib = BUF_INITIALIZER;

        annotatemore_lookup(intname, "/specialuse", userid, &attrib);

        /* \\Inbox is magical */
        if (mboxname_isusermailbox(intname, 1) &&
            mboxname_userownsmailbox(userid, intname)) {
            if (buf_len(&attrib)) buf_putc(&attrib, ' ');
            buf_appendcstr(&attrib, "\\Inbox");
        }

        if (buf_len(&attrib)) {
            strarray_t *haystack = strarray_split(buf_cstring(&attrib), " ", 0);

            for (i = 0; i < strarray_size(uses); i++) {
                if (strarray_find_case(haystack, strarray_nth(uses, i), 0) < 0) {
                    r = 0;
                    break;
                }
            }
            strarray_free(haystack);
        }
        else r = 0;

        buf_free(&attrib);
        free(intname);
    }
    else {
        for (i = 0; i < strarray_size(uses); i++) {
            char *intname = mboxlist_find_specialuse(strarray_nth(uses, i), userid);
            if (!intname) r = 0;
            free(intname);
            if (!r) break;
        }
    }

    return r;
}