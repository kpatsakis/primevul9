static apr_status_t ap_cleanup_shared_mem(void *d)
{
#if APR_HAS_SHARED_MEMORY
    free(ap_scoreboard_image);
    ap_scoreboard_image = NULL;
    apr_shm_destroy(ap_scoreboard_shm);
#endif
    return APR_SUCCESS;
}