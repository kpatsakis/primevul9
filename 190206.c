AP_DECLARE(void) ap_init_scoreboard(void *shared_score)
{
    char *more_storage;
    int i;

    pfn_ap_logio_get_last_bytes = APR_RETRIEVE_OPTIONAL_FN(ap_logio_get_last_bytes);
    if (!shared_score) {
        return;
    }
    
    ap_calc_scoreboard_size();
    ap_scoreboard_image =
        ap_calloc(1, SIZE_OF_scoreboard + server_limit * sizeof(worker_score *));
    more_storage = shared_score;
    ap_scoreboard_image->global = (global_score *)more_storage;
    more_storage += SIZE_OF_global_score;
    ap_scoreboard_image->parent = (process_score *)more_storage;
    more_storage += SIZE_OF_process_score * server_limit;
    ap_scoreboard_image->servers =
        (worker_score **)((char*)ap_scoreboard_image + SIZE_OF_scoreboard);
    for (i = 0; i < server_limit; i++) {
        ap_scoreboard_image->servers[i] = (worker_score *)more_storage;
        more_storage += thread_limit * SIZE_OF_worker_score;
    }
    ap_assert(more_storage == (char*)shared_score + scoreboard_size);
    ap_scoreboard_image->global->server_limit = server_limit;
    ap_scoreboard_image->global->thread_limit = thread_limit;
}