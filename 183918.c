void vnc_start_protocol(VncState *vs)
{
    vnc_write(vs, "RFB 003.008\n", 12);
    vnc_flush(vs);
    vnc_read_when(vs, protocol_version, 12);

    vs->mouse_mode_notifier.notify = check_pointer_type_change;
    qemu_add_mouse_mode_change_notifier(&vs->mouse_mode_notifier);
}