void krb5_auth_done(struct tevent_req *req)
{
    int ret;
    struct be_req *be_req = tevent_req_callback_data(req, struct be_req);
    int pam_status;
    int dp_err;
    struct pam_data *pd;
    struct krb5_ctx *krb5_ctx;

    pd = talloc_get_type(be_req->req_data, struct pam_data);

    ret = krb5_auth_recv(req, &pam_status, &dp_err);
    talloc_zfree(req);
    if (ret) {
        pd->pam_status = PAM_SYSTEM_ERR;
        dp_err = DP_ERR_OK;
    } else {
        pd->pam_status = pam_status;
    }

    krb5_ctx = get_krb5_ctx(be_req);
    if (krb5_ctx != NULL) {
        check_wait_queue(krb5_ctx, pd->user);
    } else {
        DEBUG(1, ("Kerberos context not available.\n"));
    }

    krb_reply(be_req, dp_err, pd->pam_status);
}