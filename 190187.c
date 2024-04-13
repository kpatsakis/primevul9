AP_DECLARE(void) ap_update_sb_handle(ap_sb_handle_t *sbh,
                                     int child_num, int thread_num)
{
    sbh->child_num = child_num;
    sbh->thread_num = thread_num;
}