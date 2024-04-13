sdap_ad_tokengroups_initgr_posix_sids_connect_done(struct tevent_req *subreq)
{
    struct sdap_ad_tokengroups_initgr_posix_state *state = NULL;
    struct tevent_req *req = NULL;
    int ret;
    int dp_error = DP_ERR_FATAL;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req,
                            struct sdap_ad_tokengroups_initgr_posix_state);


    ret = sdap_id_op_connect_recv(subreq, &dp_error);
    talloc_zfree(subreq);

    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    subreq = sdap_get_ad_tokengroups_send(state, state->ev, state->opts,
                                          sdap_id_op_handle(state->op),
                                          state->username, state->orig_dn,
                                          state->timeout);
    if (subreq == NULL) {
        tevent_req_error(req, ENOMEM);
        return;
    }

    tevent_req_set_callback(subreq, sdap_ad_tokengroups_initgr_posix_tg_done,
                            req);

    return;
}