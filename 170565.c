static int sieve_duplicate_track(void *dc,
                                 void *ic __attribute__((unused)),
                                 void *sc,
                                 void *mc __attribute__((unused)),
                                 const char **errmsg __attribute__((unused)))
{
    sieve_duplicate_context_t *dtc = (sieve_duplicate_context_t *) dc;
    script_data_t *sd = (script_data_t *) sc;
    time_t now = time(NULL);
    duplicate_key_t dkey = DUPLICATE_INITIALIZER;

    dkey.id = dtc->id;
    dkey.to = make_sieve_db(mbname_userid(sd->mbname));
    dkey.date = "";  /* no date on these, ID is custom */
    duplicate_mark(&dkey, now + dtc->seconds, 0);

    return SIEVE_OK;
}