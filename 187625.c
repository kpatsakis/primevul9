static int reset_saslconn(sasl_conn_t **conn)
{
    int ret;
    sasl_security_properties_t *secprops = NULL;

    sasl_dispose(conn);
    /* do initialization typical of service_main */
    ret = sasl_server_new("HTTP", config_servername, NULL,
                          buf_cstringnull_ifempty(&saslprops.iplocalport),
                          buf_cstringnull_ifempty(&saslprops.ipremoteport),
                          NULL, SASL_USAGE_FLAGS, conn);
    if(ret != SASL_OK) return ret;

    secprops = mysasl_secprops(0);

    /* no HTTP clients seem to use "auth-int" */
    secprops->max_ssf = 0;                              /* "auth" only */
    secprops->maxbufsize = 0;                           /* don't need maxbuf */
    ret = sasl_setprop(*conn, SASL_SEC_PROPS, secprops);
    if(ret != SASL_OK) return ret;
    /* end of service_main initialization excepting SSF */

    /* If we have TLS/SSL info, set it */
    if(saslprops.ssf) {
        ret = saslprops_set_tls(&saslprops, *conn);
    } else {
        ret = sasl_setprop(*conn, SASL_SSF_EXTERNAL, &extprops_ssf);
    }

    if(ret != SASL_OK) return ret;
    /* End TLS/SSL Info */

    return SASL_OK;
}