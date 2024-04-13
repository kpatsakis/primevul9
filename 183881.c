static void vnc_led_state_change(VncState *vs)
{
    if (!vnc_has_feature(vs, VNC_FEATURE_LED_STATE)) {
        return;
    }

    vnc_lock_output(vs);
    vnc_write_u8(vs, VNC_MSG_SERVER_FRAMEBUFFER_UPDATE);
    vnc_write_u8(vs, 0);
    vnc_write_u16(vs, 1);
    vnc_framebuffer_update(vs, 0, 0, 1, 1, VNC_ENCODING_LED_STATE);
    vnc_write_u8(vs, vs->vd->ledstate);
    vnc_unlock_output(vs);
    vnc_flush(vs);
}