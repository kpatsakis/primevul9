static int addheader(void *sc, void *mc,
                     const char *head, const char *body, int index)
{
    script_data_t *sd = (script_data_t *)sc;
    message_data_t *m = ((deliver_data_t *) mc)->m;

    if (head == NULL || body == NULL) return SIEVE_FAIL;

    if (index < 0)
        spool_append_header(xstrdup(head), xstrdup(body), m->hdrcache);
    else
        spool_prepend_header(xstrdup(head), xstrdup(body), m->hdrcache);

    sd->edited_header = 1;

    return SIEVE_OK;
}