AP_DECLARE(int) ap_calc_scoreboard_size(void)
{
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_THREADS, &thread_limit);
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &server_limit);

    scoreboard_size  = SIZE_OF_global_score;
    scoreboard_size += SIZE_OF_process_score * server_limit;
    scoreboard_size += SIZE_OF_worker_score * server_limit * thread_limit;

    return scoreboard_size;
}