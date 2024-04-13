static int sieve_duplicate_check(void *dc,
                                 void *ic __attribute__((unused)),
                                 void *sc,
                                 void *mc __attribute__((unused)),
                                 const char **errmsg __attribute__((unused)))
{
    sieve_duplicate_context_t *dtc = (sieve_duplicate_context_t *) dc;
    script_data_t *sd = (script_data_t *) sc;
    time_t t, now = time(NULL);;
    duplicate_key_t dkey = DUPLICATE_INITIALIZER;

    dkey.id = dtc->id;
    dkey.to = make_sieve_db(mbname_userid(sd->mbname));
    dkey.date = "";  /* no date on these, ID is custom */
    t = duplicate_check(&dkey);

    if (t && now < t) {
        /* active tracking record */
        duplicate_log(&dkey, "sieve-duplicate");
        return 1;
    }

    /* no active tracking record */
    return 0;
}