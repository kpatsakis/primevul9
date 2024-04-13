static int init_encoder(Encoder *encoder, QuicUsrContext *usr)
{
    int i;

    encoder->usr = usr;

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (!init_channel(encoder, &encoder->channels[i])) {
            for (--i; i >= 0; i--) {
                destroy_channel(encoder, &encoder->channels[i]);
            }
            return FALSE;
        }
    }
    return TRUE;
}