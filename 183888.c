void vnc_flush(VncState *vs)
{
    vnc_lock_output(vs);
    if (vs->ioc != NULL && vs->output.offset) {
        vnc_client_write_locked(vs);
    }
    vnc_unlock_output(vs);
}