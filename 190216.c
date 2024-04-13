AP_DECLARE(worker_score *) ap_get_scoreboard_worker(ap_sb_handle_t *sbh)
{
    if (!sbh)
        return NULL;

    return ap_get_scoreboard_worker_from_indexes(sbh->child_num,
                                                 sbh->thread_num);
}