static int getbody(void *mc, const char **content_types,
                   sieve_bodypart_t ***parts)
{
    deliver_data_t *mydata = (deliver_data_t *) mc;
    message_data_t *m = mydata->m;
    int r = 0;

    if (!mydata->content->body) {
        /* parse the message body if we haven't already */
        r = message_parse_file(m->f, &mydata->content->base,
                               &mydata->content->len, &mydata->content->body);
    }

    /* XXX currently struct bodypart as defined in message.h is the same as
       sieve_bodypart_t as defined in sieve_interface.h, so we can typecast */
    if (!r) message_fetch_part(mydata->content, content_types,
                               (struct bodypart ***) parts);
    return (!r ? SIEVE_OK : SIEVE_FAIL);
}