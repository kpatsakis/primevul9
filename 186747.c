void krb5_pam_handler(struct be_req *be_req)
{
    struct tevent_req *req;
    struct pam_data *pd;
    struct krb5_ctx *krb5_ctx;
    int dp_err = DP_ERR_FATAL;
    int ret;

    pd = talloc_get_type(be_req->req_data, struct pam_data);
    pd->pam_status = PAM_SYSTEM_ERR;

    krb5_ctx = get_krb5_ctx(be_req);
    if (krb5_ctx == NULL) {
        DEBUG(1, ("Kerberos context not available.\n"));
        goto done;
    }

    switch (pd->cmd) {
        case SSS_PAM_AUTHENTICATE:
        case SSS_CMD_RENEW:
        case SSS_PAM_CHAUTHTOK_PRELIM:
        case SSS_PAM_CHAUTHTOK:
            ret = add_to_wait_queue(be_req, pd, krb5_ctx);
            if (ret == EOK) {
                DEBUG(7, ("Request successfully added to wait queue "
                          "of user [%s].\n", pd->user));
                return;
            } else if (ret == ENOENT) {
                DEBUG(7, ("Wait queue of user [%s] is empty, "
                          "running request immediately.\n", pd->user));
            } else {
                DEBUG(7, ("Failed to add request to wait queue of user [%s], "
                          "running request immediately.\n", pd->user));
            }

            req = krb5_auth_send(be_req, be_req->be_ctx->ev, be_req->be_ctx, pd,
                                 krb5_ctx);
            if (req == NULL) {
                DEBUG(1, ("krb5_auth_send failed.\n"));
                goto done;
            }

            tevent_req_set_callback(req, krb5_auth_done, be_req);
            break;
        case SSS_PAM_ACCT_MGMT:
            req = krb5_access_send(be_req, be_req->be_ctx->ev, be_req->be_ctx,
                                   pd, krb5_ctx);
            if (req == NULL) {
                DEBUG(1, ("krb5_access_send failed.\n"));
                goto done;
            }

            tevent_req_set_callback(req, krb5_access_done, be_req);
            break;
        case SSS_PAM_SETCRED:
        case SSS_PAM_OPEN_SESSION:
        case SSS_PAM_CLOSE_SESSION:
            pd->pam_status = PAM_SUCCESS;
            dp_err = DP_ERR_OK;
            goto done;
            break;
        default:
            DEBUG(4, ("krb5 does not handles pam task %d.\n", pd->cmd));
            pd->pam_status = PAM_MODULE_UNKNOWN;
            dp_err = DP_ERR_OK;
            goto done;
    }

    return;

done:
    krb_reply(be_req, dp_err, pd->pam_status);
}