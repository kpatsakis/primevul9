AP_DECLARE(int) ap_update_child_status(ap_sb_handle_t *sbh, int status,
                                      request_rec *r)
{
    if (!sbh || (sbh->child_num < 0))
        return -1;

    return update_child_status_internal(sbh->child_num, sbh->thread_num,
                                        status,
                                        r ? r->connection : NULL,
                                        r ? r->server : NULL,
                                        r, NULL);
}