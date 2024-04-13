static int deleteheader(void *sc, void *mc, const char *head, int index)
{
    script_data_t *sd = (script_data_t *)sc;
    message_data_t *m = ((deliver_data_t *) mc)->m;

    if (head == NULL) return SIEVE_FAIL;

    if (!index) spool_remove_header(xstrdup(head), m->hdrcache);
    else spool_remove_header_instance(xstrdup(head), index, m->hdrcache);

    sd->edited_header = 1;

    return SIEVE_OK;
}