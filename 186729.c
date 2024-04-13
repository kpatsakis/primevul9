static void krb5_resolve_kdc_done(struct tevent_req *subreq)
{
    struct tevent_req *req = tevent_req_callback_data(subreq, struct tevent_req);
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);
    struct krb5child_req *kr = state->kr;
    int ret;

    ret = be_resolve_server_recv(subreq, &kr->srv);
    talloc_zfree(subreq);
    if (ret) {
        /* all servers have been tried and none
         * was found good, setting offline,
         * but we still have to call the child to setup
         * the ccache file. */
        be_mark_offline(state->be_ctx);
        kr->is_offline = true;
    } else {
        if (kr->krb5_ctx->kpasswd_service != NULL) {
            subreq = be_resolve_server_send(state, state->ev, state->be_ctx,
                                            kr->krb5_ctx->kpasswd_service->name);
            if (subreq == NULL) {
                DEBUG(1, ("be_resolve_server_send failed.\n"));
                ret = ENOMEM;
                goto failed;
            }

            tevent_req_set_callback(subreq, krb5_resolve_kpasswd_done, req);

            return;
        }
    }

    krb5_find_ccache_step(req);
    return;

failed:
    tevent_req_error(req, ret);
}