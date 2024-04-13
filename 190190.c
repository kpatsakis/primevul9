AP_DECLARE(int) ap_update_child_status_from_server(ap_sb_handle_t *sbh, int status, 
                                                   conn_rec *c, server_rec *s)
{
    if (!sbh || (sbh->child_num < 0))
        return -1;

    return update_child_status_internal(sbh->child_num, sbh->thread_num,
                                        status, c, s, NULL, NULL);
}