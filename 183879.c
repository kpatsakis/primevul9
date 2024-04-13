static void vnc_release_modifiers(VncState *vs)
{
    static const int keycodes[] = {
        /* shift, control, alt keys, both left & right */
        0x2a, 0x36, 0x1d, 0x9d, 0x38, 0xb8,
    };
    int i, keycode;

    if (!qemu_console_is_graphic(NULL)) {
        return;
    }
    for (i = 0; i < ARRAY_SIZE(keycodes); i++) {
        keycode = keycodes[i];
        if (!vs->modifiers_state[keycode]) {
            continue;
        }
        qemu_input_event_send_key_number(vs->vd->dcl.con, keycode, false);
        qemu_input_event_send_key_delay(vs->vd->key_delay_ms);
    }
}