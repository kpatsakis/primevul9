static void krb5_access_done(struct tevent_req *req)
{
    int ret;
    struct be_req *be_req = tevent_req_callback_data(req, struct be_req);
    bool access_allowed;
    struct pam_data *pd;
    int dp_err = DP_ERR_OK;

    pd = talloc_get_type(be_req->req_data, struct pam_data);
    pd->pam_status = PAM_SYSTEM_ERR;

    ret = krb5_access_recv(req, &access_allowed);
    talloc_zfree(req);
    if (ret != EOK) {
        DEBUG(1, ("krb5_access request failed [%d][%s]\n", ret, strerror(ret)));
        goto done;
    }

    DEBUG(7, ("Access %s for user [%s].\n",
              access_allowed ? "allowed" : "denied", pd->user));
    pd->pam_status = access_allowed ? PAM_SUCCESS : PAM_PERM_DENIED;
    dp_err = DP_ERR_OK;

done:
    krb_reply(be_req, dp_err, pd->pam_status);
}