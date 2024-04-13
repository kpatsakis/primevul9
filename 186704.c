static void sdap_ad_tokengroups_initgroups_done(struct tevent_req *subreq)
{
    struct sdap_ad_tokengroups_initgroups_state *state = NULL;
    struct tevent_req *req = NULL;
    errno_t ret;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_ad_tokengroups_initgroups_state);

    if (state->use_id_mapping && !IS_SUBDOMAIN(state->domain)) {
        ret = sdap_ad_tokengroups_initgr_mapping_recv(subreq);
    } else {
        ret = sdap_ad_tokengroups_initgr_posix_recv(subreq);
    }
    talloc_zfree(subreq);
    if (ret != EOK) {
        goto done;
    }

done:
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}