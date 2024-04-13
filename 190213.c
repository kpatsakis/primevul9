AP_DECLARE(void) ap_create_sb_handle(ap_sb_handle_t **new_sbh, apr_pool_t *p,
                                     int child_num, int thread_num)
{
    *new_sbh = (ap_sb_handle_t *)apr_palloc(p, sizeof(ap_sb_handle_t));
    ap_update_sb_handle(*new_sbh, child_num, thread_num);
}