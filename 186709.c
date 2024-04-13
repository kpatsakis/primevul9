static errno_t sdap_get_ad_tokengroups_recv(TALLOC_CTX *mem_ctx,
                                            struct tevent_req *req,
                                            size_t *_num_sids,
                                            char ***_sids)
{
    struct sdap_get_ad_tokengroups_state *state = NULL;
    state = tevent_req_data(req, struct sdap_get_ad_tokengroups_state);

    TEVENT_REQ_RETURN_ON_ERROR(req);

    if (_num_sids != NULL) {
        *_num_sids = state->num_sids;
    }

    if (_sids != NULL) {
        *_sids = talloc_steal(mem_ctx, state->sids);
    }

    return EOK;
}