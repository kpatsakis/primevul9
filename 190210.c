apr_status_t ap_cleanup_scoreboard(void *d)
{
    if (ap_scoreboard_image == NULL) {
        return APR_SUCCESS;
    }
    if (scoreboard_type == SB_SHARED) {
        ap_cleanup_shared_mem(NULL);
    }
    else {
        free(ap_scoreboard_image->global);
        free(ap_scoreboard_image);
        ap_scoreboard_image = NULL;
    }
    return APR_SUCCESS;
}