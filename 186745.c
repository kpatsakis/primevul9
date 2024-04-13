static struct tevent_req *krb5_next_kdc(struct tevent_req *req)
{
    struct tevent_req *next_req;
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);

    next_req = be_resolve_server_send(state, state->ev,
                                      state->be_ctx,
                                      state->krb5_ctx->service->name);
    if (next_req == NULL) {
        DEBUG(1, ("be_resolve_server_send failed.\n"));
        return NULL;
    }
    tevent_req_set_callback(next_req, krb5_resolve_kdc_done, req);

    return next_req;
}