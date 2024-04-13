static void krb_reply(struct be_req *req, int dp_err, int result)
{
    req->fn(req, dp_err, result, NULL);
}