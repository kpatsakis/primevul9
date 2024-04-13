static gboolean vnc_listen_io(QIOChannel *ioc,
                              GIOCondition condition,
                              void *opaque)
{
    VncDisplay *vd = opaque;
    QIOChannelSocket *sioc = NULL;
    Error *err = NULL;
    bool isWebsock = false;
    size_t i;

    for (i = 0; i < vd->nlwebsock; i++) {
        if (ioc == QIO_CHANNEL(vd->lwebsock[i])) {
            isWebsock = true;
            break;
        }
    }

    sioc = qio_channel_socket_accept(QIO_CHANNEL_SOCKET(ioc), &err);
    if (sioc != NULL) {
        qio_channel_set_name(QIO_CHANNEL(sioc),
                             isWebsock ? "vnc-ws-server" : "vnc-server");
        qio_channel_set_delay(QIO_CHANNEL(sioc), false);
        vnc_connect(vd, sioc, false, isWebsock);
        object_unref(OBJECT(sioc));
    } else {
        /* client probably closed connection before we got there */
        error_free(err);
    }

    return TRUE;
}