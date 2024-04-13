QuicContext *quic_create(QuicUsrContext *usr)
{
    Encoder *encoder;

    if (!usr || !usr->error || !usr->warn || !usr->info || !usr->malloc ||
        !usr->free || !usr->more_space || !usr->more_lines) {
        return NULL;
    }

    if (!(encoder = (Encoder *)usr->malloc(usr, sizeof(Encoder)))) {
        return NULL;
    }

    if (!init_encoder(encoder, usr)) {
        usr->free(usr, encoder);
        return NULL;
    }
    return (QuicContext *)encoder;
}