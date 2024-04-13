static void vnc_client_write(VncState *vs)
{

    vnc_lock_output(vs);
    if (vs->output.offset) {
        vnc_client_write_locked(vs);
    } else if (vs->ioc != NULL) {
        if (vs->ioc_tag) {
            g_source_remove(vs->ioc_tag);
        }
        vs->ioc_tag = qio_channel_add_watch(
            vs->ioc, G_IO_IN, vnc_client_io, vs, NULL);
    }
    vnc_unlock_output(vs);
}