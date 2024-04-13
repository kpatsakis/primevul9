static void kbd_leds(void *opaque, int ledstate)
{
    VncDisplay *vd = opaque;
    VncState *client;

    trace_vnc_key_guest_leds((ledstate & QEMU_CAPS_LOCK_LED),
                             (ledstate & QEMU_NUM_LOCK_LED),
                             (ledstate & QEMU_SCROLL_LOCK_LED));

    if (ledstate == vd->ledstate) {
        return;
    }

    vd->ledstate = ledstate;

    QTAILQ_FOREACH(client, &vd->clients, next) {
        vnc_led_state_change(client);
    }
}