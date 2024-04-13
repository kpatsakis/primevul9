static VncClientInfo *qmp_query_vnc_client(const VncState *client)
{
    VncClientInfo *info;
    Error *err = NULL;

    info = g_malloc0(sizeof(*info));

    vnc_init_basic_info_from_remote_addr(client->sioc,
                                         qapi_VncClientInfo_base(info),
                                         &err);
    if (err) {
        error_free(err);
        qapi_free_VncClientInfo(info);
        return NULL;
    }

    info->websocket = client->websocket;

    if (client->tls) {
        info->x509_dname = qcrypto_tls_session_get_peer_name(client->tls);
        info->has_x509_dname = info->x509_dname != NULL;
    }
#ifdef CONFIG_VNC_SASL
    if (client->sasl.conn && client->sasl.username) {
        info->has_sasl_username = true;
        info->sasl_username = g_strdup(client->sasl.username);
    }
#endif

    return info;
}