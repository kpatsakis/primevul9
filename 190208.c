static apr_status_t create_namebased_scoreboard(apr_pool_t *pool,
                                                const char *fname)
{
#if APR_HAS_SHARED_MEMORY
    apr_status_t rv;

    /* The shared memory file must not exist before we create the
     * segment. */
    apr_shm_remove(fname, pool); /* ignore errors */

    rv = apr_shm_create(&ap_scoreboard_shm, scoreboard_size, fname, pool);
    if (rv != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_CRIT, rv, ap_server_conf, APLOGNO(00001)
                     "unable to create or access scoreboard \"%s\" "
                     "(name-based shared memory failure)", fname);
        return rv;
    }
#endif /* APR_HAS_SHARED_MEMORY */
    return APR_SUCCESS;
}