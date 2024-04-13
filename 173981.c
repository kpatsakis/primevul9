void quic_destroy(QuicContext *quic)
{
    Encoder *encoder = (Encoder *)quic;
    int i;

    if (!quic) {
        return;
    }

    for (i = 0; i < MAX_CHANNELS; i++) {
        destroy_channel(encoder, &encoder->channels[i]);
    }
    encoder->usr->free(encoder->usr, encoder);
}