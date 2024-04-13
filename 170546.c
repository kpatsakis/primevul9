static int getenvelope(void *mc, const char *field, const char ***contents)
{
    deliver_data_t *mydata = (deliver_data_t *) mc;
    message_data_t *m = mydata->m;

    if (!strcasecmp(field, "from")) {
        *contents = mydata->temp;
        mydata->temp[0] = m->return_path;
        mydata->temp[1] = NULL;
        return SIEVE_OK;
    } else if (!strcasecmp(field, "to")) {
        *contents = mydata->temp;
        mydata->temp[0] = msg_getrcptall(m, mydata->cur_rcpt);
        mydata->temp[1] = NULL;
        return SIEVE_OK;
    } else if (!strcasecmp(field, "auth") && mydata->authuser) {
        *contents = mydata->temp;
        mydata->temp[0] = mydata->authuser;
        mydata->temp[1] = NULL;
        return SIEVE_OK;
    } else {
        *contents = NULL;
        return SIEVE_FAIL;
    }
}