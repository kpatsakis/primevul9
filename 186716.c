sdap_ad_tokengroups_initgr_posix_tg_done(struct tevent_req *subreq)
{
    struct sdap_ad_tokengroups_initgr_posix_state *state = NULL;
    struct tevent_req *req = NULL;
    char **sids = NULL;
    size_t num_sids = 0;
    errno_t ret;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_ad_tokengroups_initgr_posix_state);

    ret = sdap_get_ad_tokengroups_recv(state, subreq, &num_sids, &sids);
    talloc_zfree(subreq);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Unable to acquire tokengroups [%d]: %s\n",
                                    ret, strerror(ret));
        goto done;
    }

    ret = sdap_ad_tokengroups_get_posix_members(state, state,
                                                num_sids, sids,
                                                &state->num_missing_sids,
                                                &state->missing_sids,
                                                &state->num_cached_groups,
                                                &state->cached_groups);
    if (ret != EOK) {
        goto done;
    }

    /* download missing SIDs */
    subreq = sdap_ad_resolve_sids_send(state, state->ev, state->id_ctx,
                                       state->conn,
                                       state->opts, state->domain,
                                       state->missing_sids);
    if (subreq == NULL) {
        ret = ENOMEM;
        goto done;
    }

    tevent_req_set_callback(subreq, sdap_ad_tokengroups_initgr_posix_sids_done,
                            req);

    return;

done:
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}