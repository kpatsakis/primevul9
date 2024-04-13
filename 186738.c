static void krb5_pam_handler_cache_auth_step(struct tevent_req *req)
{
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);
    struct pam_data *pd = state->pd;
    struct krb5_ctx *krb5_ctx = state->kr->krb5_ctx;
    int ret;

    ret = sysdb_cache_auth(state, state->be_ctx->sysdb, state->be_ctx->domain,
                           pd->user, pd->authtok, pd->authtok_size,
                           state->be_ctx->cdb, true, NULL, NULL);
    if (ret != EOK) {
        DEBUG(1, ("Offline authentication failed\n"));
        state->pam_status = PAM_SYSTEM_ERR;
        state->dp_err = DP_ERR_OK;
    } else {
        ret = add_user_to_delayed_online_authentication(krb5_ctx, pd,
                                                       state->kr->uid);
        if (ret != EOK) {
            /* This error is not fatal */
            DEBUG(1, ("add_user_to_delayed_online_authentication failed.\n"));
        }
        state->pam_status = PAM_AUTHINFO_UNAVAIL;
        state->dp_err = DP_ERR_OFFLINE;
    }

    tevent_req_done(req);
}