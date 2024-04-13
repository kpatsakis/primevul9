void vnc_display_init(const char *id)
{
    VncDisplay *vd;

    if (vnc_display_find(id) != NULL) {
        return;
    }
    vd = g_malloc0(sizeof(*vd));

    vd->id = strdup(id);
    QTAILQ_INSERT_TAIL(&vnc_displays, vd, next);

    QTAILQ_INIT(&vd->clients);
    vd->expires = TIME_MAX;

    if (keyboard_layout) {
        trace_vnc_key_map_init(keyboard_layout);
        vd->kbd_layout = init_keyboard_layout(name2keysym, keyboard_layout);
    } else {
        vd->kbd_layout = init_keyboard_layout(name2keysym, "en-us");
    }

    if (!vd->kbd_layout) {
        exit(1);
    }

    vd->share_policy = VNC_SHARE_POLICY_ALLOW_EXCLUSIVE;
    vd->connections_limit = 32;

    qemu_mutex_init(&vd->mutex);
    vnc_start_worker_thread();

    vd->dcl.ops = &dcl_ops;
    register_displaychangelistener(&vd->dcl);
}