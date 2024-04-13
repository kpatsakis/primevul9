static int getsize(void *mc, int *size)
{
    message_data_t *m = ((deliver_data_t *) mc)->m;

    *size = msg_getsize(m);
    return SIEVE_OK;
}