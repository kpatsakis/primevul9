static struct krb5_ctx *get_krb5_ctx(struct be_req *be_req)
{
    struct pam_data *pd;

    pd = talloc_get_type(be_req->req_data, struct pam_data);

    switch (pd->cmd) {
        case SSS_PAM_AUTHENTICATE:
        case SSS_CMD_RENEW:
            return talloc_get_type(be_req->be_ctx->bet_info[BET_AUTH].pvt_bet_data,
                                   struct krb5_ctx);
            break;
        case SSS_PAM_ACCT_MGMT:
            return talloc_get_type(be_req->be_ctx->bet_info[BET_ACCESS].pvt_bet_data,
                                   struct krb5_ctx);
            break;
        case SSS_PAM_CHAUTHTOK:
        case SSS_PAM_CHAUTHTOK_PRELIM:
            return talloc_get_type(be_req->be_ctx->bet_info[BET_CHPASS].pvt_bet_data,
                                   struct krb5_ctx);
            break;
        default:
            DEBUG(1, ("Unsupported PAM task.\n"));
            return NULL;
    }
}