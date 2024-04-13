int ap_create_scoreboard(apr_pool_t *p, ap_scoreboard_e sb_type)
{
    int i;
#if APR_HAS_SHARED_MEMORY
    apr_status_t rv;
#endif

    if (ap_scoreboard_image) {
        ap_scoreboard_image->global->restart_time = apr_time_now();
        memset(ap_scoreboard_image->parent, 0,
               SIZE_OF_process_score * server_limit);
        for (i = 0; i < server_limit; i++) {
            memset(ap_scoreboard_image->servers[i], 0,
                   SIZE_OF_worker_score * thread_limit);
        }
        ap_init_scoreboard(NULL);
        return OK;
    }

    ap_calc_scoreboard_size();
#if APR_HAS_SHARED_MEMORY
    if (sb_type == SB_SHARED) {
        void *sb_shared;
        rv = open_scoreboard(p);
        if (rv || !(sb_shared = apr_shm_baseaddr_get(ap_scoreboard_shm))) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        memset(sb_shared, 0, scoreboard_size);
        ap_init_scoreboard(sb_shared);
    }
    else
#endif
    {
        /* A simple malloc will suffice */
        void *sb_mem = ap_calloc(1, scoreboard_size);
        ap_init_scoreboard(sb_mem);
    }

    scoreboard_type = sb_type;
    ap_scoreboard_image->global->running_generation = 0;
    ap_scoreboard_image->global->restart_time = apr_time_now();

    apr_pool_cleanup_register(p, NULL, ap_cleanup_scoreboard, apr_pool_cleanup_null);

    return OK;
}