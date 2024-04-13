static int jmapquery(void *sc, void *mc, const char *json)
{
    script_data_t *sd = (script_data_t *) sc;
    message_data_t *md = ((deliver_data_t *) mc)->m;
    struct buf msg = BUF_INITIALIZER;
    const char *userid = mbname_userid(sd->mbname);
    json_error_t jerr;
    json_t *jfilter, *err = NULL;
    int r;

    /* Create filter from json */
    jfilter = json_loads(json, 0, &jerr);
    if (!jfilter) return 0;

    /* mmap the staged message file */
    buf_refresh_mmap(&msg, 1, fileno(md->f), md->id, md->size, NULL);

    /* Run query */
    r = jmap_email_matchmime(&msg, jfilter, userid, time(NULL), &err);

    if (err) {
        char *errstr = json_dumps(err, JSON_COMPACT);
        syslog(LOG_ERR, "sieve: jmapquery error: %s", errstr);
        free(errstr);
        r = SIEVE_RUN_ERROR;
    }

    json_decref(jfilter);
    buf_free(&msg);

    return r;
}