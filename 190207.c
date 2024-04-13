AP_DECLARE(void) ap_time_process_request(ap_sb_handle_t *sbh, int status)
{
    worker_score *ws;

    if (!sbh)
        return;

    if (sbh->child_num < 0) {
        return;
    }

    ws = &ap_scoreboard_image->servers[sbh->child_num][sbh->thread_num];

    if (status == START_PREQUEST) {
        ws->start_time = ws->last_used = apr_time_now();
    }
    else if (status == STOP_PREQUEST) {
        ws->stop_time = ws->last_used = apr_time_now();
        if (ap_extended_status) {
            ws->duration += ws->stop_time - ws->start_time;
        }
    }
}