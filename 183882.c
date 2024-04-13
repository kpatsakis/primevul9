static void vnc_client_cache_addr(VncState *client)
{
    Error *err = NULL;

    client->info = g_malloc0(sizeof(*client->info));
    vnc_init_basic_info_from_remote_addr(client->sioc,
                                         qapi_VncClientInfo_base(client->info),
                                         &err);
    if (err) {
        qapi_free_VncClientInfo(client->info);
        client->info = NULL;
        error_free(err);
    }
}