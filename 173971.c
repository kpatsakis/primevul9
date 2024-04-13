static void uncompress_gray(Encoder *encoder, uint8_t *buf, int stride)
{
    unsigned int row;
    uint8_t *prev;

    encoder->channels[0].correlate_row[-1] = 0;
    quic_one_uncompress_row0(encoder, &encoder->channels[0], (one_byte_t *)buf, encoder->width);
    encoder->rows_completed++;
    for (row = 1; row < encoder->height; row++) {
        prev = buf;
        buf += stride;
        encoder->channels[0].correlate_row[-1] = encoder->channels[0].correlate_row[0];
        quic_one_uncompress_row(encoder, &encoder->channels[0], (one_byte_t *)prev,
                                (one_byte_t *)buf, encoder->width);
        encoder->rows_completed++;
    }
}