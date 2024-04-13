static void vnc_client_write_locked(VncState *vs)
{
#ifdef CONFIG_VNC_SASL
    if (vs->sasl.conn &&
        vs->sasl.runSSF &&
        !vs->sasl.waitWriteSSF) {
        vnc_client_write_sasl(vs);
    } else
#endif /* CONFIG_VNC_SASL */
    {
        vnc_client_write_plain(vs);
    }
}