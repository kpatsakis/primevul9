void vnc_display_add_client(const char *id, int csock, bool skipauth)
{
    VncDisplay *vd = vnc_display_find(id);
    QIOChannelSocket *sioc;

    if (!vd) {
        return;
    }

    sioc = qio_channel_socket_new_fd(csock, NULL);
    if (sioc) {
        qio_channel_set_name(QIO_CHANNEL(sioc), "vnc-server");
        vnc_connect(vd, sioc, skipauth, false);
        object_unref(OBJECT(sioc));
    }
}