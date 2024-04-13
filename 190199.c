AP_DECLARE(int) ap_update_global_status()
{
#ifdef HAVE_TIMES
    if (ap_scoreboard_image == NULL) {
        return APR_SUCCESS;
    }
    times(&ap_scoreboard_image->global->times);
#endif
    return APR_SUCCESS;
}