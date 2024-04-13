AP_DECLARE(void) ap_copy_scoreboard_worker(worker_score *dest, 
                                           int child_num,
                                           int thread_num)
{
    worker_score *ws = ap_get_scoreboard_worker_from_indexes(child_num, thread_num);

    memcpy(dest, ws, sizeof *ws);

    /* For extra safety, NUL-terminate the strings returned, though it
     * should be true those last bytes are always zero anyway. */
    dest->client[sizeof(dest->client) - 1] = '\0';
    dest->client64[sizeof(dest->client64) - 1] = '\0';
    dest->request[sizeof(dest->request) - 1] = '\0';
    dest->vhost[sizeof(dest->vhost) - 1] = '\0';
    dest->protocol[sizeof(dest->protocol) - 1] = '\0';
}