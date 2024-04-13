AP_DECLARE(void) ap_set_conn_count(ap_sb_handle_t *sb, request_rec *r, 
                                   unsigned short conn_count)
{
    worker_score *ws;

    if (!sb)
        return;

    ws = &ap_scoreboard_image->servers[sb->child_num][sb->thread_num];
    ws->conn_count = conn_count;
}