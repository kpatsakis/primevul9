AP_DECLARE(int) ap_update_child_status_descr(ap_sb_handle_t *sbh, int status, const char *descr)
{
    if (!sbh || (sbh->child_num < 0))
        return -1;

    return update_child_status_internal(sbh->child_num, sbh->thread_num,
                                        status, NULL, NULL, NULL, descr);
}