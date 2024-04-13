static int list_addresses(void *rock, struct carddav_data *cdata)
{
    smtp_envelope_t *sm_env = rock;
    int i;

    /* XXX  Lookup up emails for vcard */
    if (!cdata->emails) return 0;
    for (i = 0; i < strarray_size(cdata->emails); i++) {
        /* Find preferred address */
        smtp_envelope_add_rcpt(sm_env, strarray_nth(cdata->emails, i));
    }

    return 0;
}