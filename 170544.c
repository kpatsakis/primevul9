static int getheader(void *v, const char *phead, const char ***body)
{
    message_data_t *m = ((deliver_data_t *) v)->m;

    if (phead==NULL) return SIEVE_FAIL;
    *body = msg_getheader(m, phead);

    if (*body) {
        return SIEVE_OK;
    } else {
        return SIEVE_FAIL;
    }
}