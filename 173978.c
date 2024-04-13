int quic_encode(QuicContext *quic, QuicImageType type, int width, int height,
                uint8_t *line, unsigned int num_lines, int stride,
                uint32_t *io_ptr, unsigned int num_io_words)
{
    Encoder *encoder = (Encoder *)quic;
    uint32_t *io_ptr_end = io_ptr + num_io_words;
    uint8_t *lines_end;
    int row;
    uint8_t *prev;
    int channels;
    int bpc;

    lines_end = line + num_lines * stride;
    if (line == NULL && lines_end != line) {
        spice_warn_if_reached();
        return QUIC_ERROR;
    }

    quic_image_params(encoder, type, &channels, &bpc);

    if (!encoder_reset(encoder, io_ptr, io_ptr_end) ||
        !encoder_reset_channels(encoder, channels, width, bpc)) {
        return QUIC_ERROR;
    }

    encoder->io_word = 0;
    encoder->io_available_bits = 32;

    encode_32(encoder, QUIC_MAGIC);
    encode_32(encoder, QUIC_VERSION);
    encode_32(encoder, type);
    encode_32(encoder, width);
    encode_32(encoder, height);

    FILL_LINES();

    switch (type) {
    case QUIC_IMAGE_TYPE_RGB32:
        spice_assert(ABS(stride) >= width * 4);
        QUIC_COMPRESS_RGB(32);
        break;
    case QUIC_IMAGE_TYPE_RGB24:
        spice_assert(ABS(stride) >= width * 3);
        QUIC_COMPRESS_RGB(24);
        break;
    case QUIC_IMAGE_TYPE_RGB16:
        spice_assert(ABS(stride) >= width * 2);
        QUIC_COMPRESS_RGB(16);
        break;
    case QUIC_IMAGE_TYPE_RGBA:
        spice_assert(ABS(stride) >= width * 4);

        encoder->channels[0].correlate_row[-1] = 0;
        encoder->channels[1].correlate_row[-1] = 0;
        encoder->channels[2].correlate_row[-1] = 0;
        quic_rgb32_compress_row0(encoder, (rgb32_pixel_t *)(line), width);

        encoder->channels[3].correlate_row[-1] = 0;
        quic_four_compress_row0(encoder, &encoder->channels[3], (four_bytes_t *)(line + 3), width);

        encoder->rows_completed++;

        for (row = 1; row < height; row++) {
            prev = line;
            NEXT_LINE();
            encoder->channels[0].correlate_row[-1] = encoder->channels[0].correlate_row[0];
            encoder->channels[1].correlate_row[-1] = encoder->channels[1].correlate_row[0];
            encoder->channels[2].correlate_row[-1] = encoder->channels[2].correlate_row[0];
            quic_rgb32_compress_row(encoder, (rgb32_pixel_t *)prev, (rgb32_pixel_t *)line, width);

            encoder->channels[3].correlate_row[-1] = encoder->channels[3].correlate_row[0];
            quic_four_compress_row(encoder, &encoder->channels[3], (four_bytes_t *)(prev + 3),
                                   (four_bytes_t *)(line + 3), width);
            encoder->rows_completed++;
        }
        break;
    case QUIC_IMAGE_TYPE_GRAY:
        spice_assert(ABS(stride) >= width);
        encoder->channels[0].correlate_row[-1] = 0;
        quic_one_compress_row0(encoder, &encoder->channels[0], (one_byte_t *)line, width);
        encoder->rows_completed++;
        for (row = 1; row < height; row++) {
            prev = line;
            NEXT_LINE();
            encoder->channels[0].correlate_row[-1] = encoder->channels[0].correlate_row[0];
            quic_one_compress_row(encoder, &encoder->channels[0], (one_byte_t *)prev,
                                  (one_byte_t *)line, width);
            encoder->rows_completed++;
        }
        break;
    case QUIC_IMAGE_TYPE_INVALID:
    default:
        encoder->usr->error(encoder->usr, "bad image type\n");
    }

    flush(encoder);
    encoder->io_words_count -= (encoder->io_end - encoder->io_now);

    return encoder->io_words_count;
}