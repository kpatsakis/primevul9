static struct tevent_req *krb5_next_server(struct tevent_req *req)
{
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);
    struct pam_data *pd = state->pd;
    struct tevent_req *next_req = NULL;

    switch (pd->cmd) {
        case SSS_PAM_AUTHENTICATE:
        case SSS_CMD_RENEW:
            fo_set_port_status(state->kr->srv, PORT_NOT_WORKING);
            next_req = krb5_next_kdc(req);
            break;
        case SSS_PAM_CHAUTHTOK:
        case SSS_PAM_CHAUTHTOK_PRELIM:
            if (state->kr->kpasswd_srv) {
                fo_set_port_status(state->kr->kpasswd_srv, PORT_NOT_WORKING);
                next_req = krb5_next_kpasswd(req);
                break;
            } else {
                fo_set_port_status(state->kr->srv, PORT_NOT_WORKING);
                next_req = krb5_next_kdc(req);
                break;
            }
        default:
            DEBUG(1, ("Unexpected PAM task\n"));
    }

    return next_req;
}