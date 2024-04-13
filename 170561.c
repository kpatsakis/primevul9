static int sieve_keep(void *ac,
                      void *ic __attribute__((unused)),
                      void *sc, void *mc, const char **errmsg)
{
    sieve_keep_context_t *kc = (sieve_keep_context_t *) ac;
    script_data_t *sd = (script_data_t *) sc;
    deliver_data_t *mydata = (deliver_data_t *) mc;
    struct imap4flags imap4flags = { kc->imapflags, sd->authstate };
    int ret;

    if (sd->edited_header) {
        mydata = setup_special_delivery(mydata);
        if (!mydata) return SIEVE_FAIL;
    }

    ret = deliver_local(mydata, &imap4flags, sd->mbname);

    if (sd->edited_header) cleanup_special_delivery(mydata);
 
    if (!ret) {
        prometheus_increment(CYRUS_LMTP_SIEVE_KEEP_TOTAL);
        snmp_increment(SIEVE_KEEP, 1);
        return SIEVE_OK;
    } else {
        *errmsg = error_message(ret);
        return SIEVE_FAIL;
    }
}