sdap_ad_tokengroups_initgr_posix_sids_done(struct tevent_req *subreq)
{
    struct sdap_ad_tokengroups_initgr_posix_state *state = NULL;
    struct tevent_req *req = NULL;
    errno_t ret;
    char **cached_groups;
    size_t num_cached_groups;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_ad_tokengroups_initgr_posix_state);

    ret = sdap_ad_resolve_sids_recv(subreq);
    talloc_zfree(subreq);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Unable to resolve missing SIDs "
                                   "[%d]: %s\n", ret, strerror(ret));
        goto done;
    }

    ret = sdap_ad_tokengroups_get_posix_members(state, state,
                                                state->num_missing_sids,
                                                state->missing_sids,
                                                NULL, NULL,
                                                &num_cached_groups,
                                                &cached_groups);
    if (ret != EOK){
        DEBUG(SSSDBG_MINOR_FAILURE,
              "sdap_ad_tokengroups_get_posix_members failed [%d]: %s\n",
              ret, strerror(ret));
        goto done;
    }

    state->cached_groups = concatenate_string_array(state,
                                                    state->cached_groups,
                                                    state->num_cached_groups,
                                                    cached_groups,
                                                    num_cached_groups);
    if (state->cached_groups == NULL) {
        ret = ENOMEM;
        goto done;
    }

    /* update membership of existing groups */
    ret = sdap_ad_tokengroups_update_members(state,
                                             state->username,
                                             state->sysdb, state->domain,
                                             state->cached_groups);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Membership update failed [%d]: %s\n",
                                     ret, strerror(ret));
        goto done;
    }

done:
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}