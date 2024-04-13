gboolean vnc_client_io(QIOChannel *ioc G_GNUC_UNUSED,
                       GIOCondition condition, void *opaque)
{
    VncState *vs = opaque;
    if (condition & G_IO_IN) {
        if (vnc_client_read(vs) < 0) {
            return TRUE;
        }
    }
    if (condition & G_IO_OUT) {
        vnc_client_write(vs);
    }
    return TRUE;
}