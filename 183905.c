static void reset_keys(VncState *vs)
{
    int i;
    for(i = 0; i < 256; i++) {
        if (vs->modifiers_state[i]) {
            qemu_input_event_send_key_number(vs->vd->dcl.con, i, false);
            qemu_input_event_send_key_delay(vs->vd->key_delay_ms);
            vs->modifiers_state[i] = 0;
        }
    }
}