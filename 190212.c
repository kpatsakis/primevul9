static apr_status_t open_scoreboard(apr_pool_t *pconf)
{
#if APR_HAS_SHARED_MEMORY
    apr_status_t rv;
    char *fname = NULL;
    apr_pool_t *global_pool;

    /* We don't want to have to recreate the scoreboard after
     * restarts, so we'll create a global pool and never clean it.
     */
    rv = apr_pool_create(&global_pool, NULL);
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf, APLOGNO(00002)
                     "Fatal error: unable to create global pool "
                     "for use by the scoreboard");
        return rv;
    }

    /* The config says to create a name-based shmem */
    if (ap_scoreboard_fname) {
        /* make sure it's an absolute pathname */
        fname = ap_server_root_relative(pconf, ap_scoreboard_fname);
        if (!fname) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, APR_EBADPATH, ap_server_conf, APLOGNO(00003)
                         "Fatal error: Invalid Scoreboard path %s",
                         ap_scoreboard_fname);
            return APR_EBADPATH;
        }
        return create_namebased_scoreboard(global_pool, fname);
    }
    else { /* config didn't specify, we get to choose shmem type */
        rv = apr_shm_create(&ap_scoreboard_shm, scoreboard_size, NULL,
                            global_pool); /* anonymous shared memory */
        if ((rv != APR_SUCCESS) && (rv != APR_ENOTIMPL)) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf, APLOGNO(00004)
                         "Unable to create or access scoreboard "
                         "(anonymous shared memory failure)");
            return rv;
        }
        /* Make up a filename and do name-based shmem */
        else if (rv == APR_ENOTIMPL) {
            /* Make sure it's an absolute pathname */
            ap_scoreboard_fname = DEFAULT_SCOREBOARD;
            fname = ap_server_root_relative(pconf, ap_scoreboard_fname);

            return create_namebased_scoreboard(global_pool, fname);
        }
    }
#endif /* APR_HAS_SHARED_MEMORY */
    return APR_SUCCESS;
}