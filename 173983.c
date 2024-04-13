static void uncompress_rgba(Encoder *encoder, uint8_t *buf, int stride)
{
    unsigned int row;
    uint8_t *prev;

    encoder->channels[0].correlate_row[-1] = 0;
    encoder->channels[1].correlate_row[-1] = 0;
    encoder->channels[2].correlate_row[-1] = 0;
    quic_rgb32_uncompress_row0(encoder, (rgb32_pixel_t *)buf, encoder->width);

    encoder->channels[3].correlate_row[-1] = 0;
    quic_four_uncompress_row0(encoder, &encoder->channels[3], (four_bytes_t *)(buf + 3),
                              encoder->width);

    encoder->rows_completed++;
    for (row = 1; row < encoder->height; row++) {
        prev = buf;
        buf += stride;

        encoder->channels[0].correlate_row[-1] = encoder->channels[0].correlate_row[0];
        encoder->channels[1].correlate_row[-1] = encoder->channels[1].correlate_row[0];
        encoder->channels[2].correlate_row[-1] = encoder->channels[2].correlate_row[0];
        quic_rgb32_uncompress_row(encoder, (rgb32_pixel_t *)prev, (rgb32_pixel_t *)buf,
                                  encoder->width);

        encoder->channels[3].correlate_row[-1] = encoder->channels[3].correlate_row[0];
        quic_four_uncompress_row(encoder, &encoder->channels[3], (four_bytes_t *)(prev + 3),
                                 (four_bytes_t *)(buf + 3), encoder->width);

        encoder->rows_completed++;
    }
}