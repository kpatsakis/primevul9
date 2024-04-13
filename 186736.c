int krb5_auth_recv(struct tevent_req *req, int *pam_status, int *dp_err)
{
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);

    *pam_status = state->pam_status;
    *dp_err = state->dp_err;

    TEVENT_REQ_RETURN_ON_ERROR(req);

    return EOK;
}