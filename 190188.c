AP_DECLARE(int) ap_update_child_status_from_indexes(int child_num,
                                                    int thread_num,
                                                    int status,
                                                    request_rec *r)
{
    if (child_num < 0) {
        return -1;
    }

    return update_child_status_internal(child_num, thread_num, status,
                                        r ? r->connection : NULL,
                                        r ? r->server : NULL,
                                        r, NULL);
}