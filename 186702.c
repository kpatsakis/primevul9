static void sdap_ad_resolve_sids_done(struct tevent_req *subreq)
{
    struct sdap_ad_resolve_sids_state *state = NULL;
    struct tevent_req *req = NULL;
    int dp_error;
    int sdap_error;
    errno_t ret;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_ad_resolve_sids_state);

    ret = groups_get_recv(subreq, &dp_error, &sdap_error);
    talloc_zfree(subreq);

    if (ret == EOK && sdap_error == ENOENT && dp_error == DP_ERR_OK) {
        /* Group was not found, we will ignore the error and continue with
         * next group. This may happen for example if the group is built-in,
         * but a custom search base is provided. */
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Unable to resolve SID %s - will try next sid.\n",
              state->current_sid);
    } else if (ret != EOK || sdap_error != EOK || dp_error != DP_ERR_OK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Unable to resolve SID %s [dp_error: %d, "
              "sdap_error: %d, ret: %d]: %s\n", state->current_sid, dp_error,
              sdap_error, ret, strerror(ret));
        goto done;
    }

    ret = sdap_ad_resolve_sids_step(req);
    if (ret == EAGAIN) {
        /* continue with next SID */
        return;
    }

done:
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}