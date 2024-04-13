int quic_decode(QuicContext *quic, QuicImageType type, uint8_t *buf, int stride)
{
    Encoder *encoder = (Encoder *)quic;
    unsigned int row;
    uint8_t *prev;

    spice_assert(buf);

    switch (encoder->type) {
    case QUIC_IMAGE_TYPE_RGB32:
    case QUIC_IMAGE_TYPE_RGB24:
        if (type == QUIC_IMAGE_TYPE_RGB32) {
            spice_assert(ABS(stride) >= (int)encoder->width * 4);
            QUIC_UNCOMPRESS_RGB(32, rgb32_pixel_t);
            break;
        } else if (type == QUIC_IMAGE_TYPE_RGB24) {
            spice_assert(ABS(stride) >= (int)encoder->width * 3);
            QUIC_UNCOMPRESS_RGB(24, rgb24_pixel_t);
            break;
        }
        encoder->usr->warn(encoder->usr, "unsupported output format\n");
        return QUIC_ERROR;
    case QUIC_IMAGE_TYPE_RGB16:
        if (type == QUIC_IMAGE_TYPE_RGB16) {
            spice_assert(ABS(stride) >= (int)encoder->width * 2);
            QUIC_UNCOMPRESS_RGB(16, rgb16_pixel_t);
        } else if (type == QUIC_IMAGE_TYPE_RGB32) {
            spice_assert(ABS(stride) >= (int)encoder->width * 4);
            QUIC_UNCOMPRESS_RGB(16_to_32, rgb32_pixel_t);
        } else {
            encoder->usr->warn(encoder->usr, "unsupported output format\n");
            return QUIC_ERROR;
        }

        break;
    case QUIC_IMAGE_TYPE_RGBA:

        if (type != QUIC_IMAGE_TYPE_RGBA) {
            encoder->usr->warn(encoder->usr, "unsupported output format\n");
            return QUIC_ERROR;
        }
        spice_assert(ABS(stride) >= (int)encoder->width * 4);
        uncompress_rgba(encoder, buf, stride);
        break;
    case QUIC_IMAGE_TYPE_GRAY:

        if (type != QUIC_IMAGE_TYPE_GRAY) {
            encoder->usr->warn(encoder->usr, "unsupported output format\n");
            return QUIC_ERROR;
        }
        spice_assert(ABS(stride) >= (int)encoder->width);
        uncompress_gray(encoder, buf, stride);
        break;
    case QUIC_IMAGE_TYPE_INVALID:
    default:
        encoder->usr->error(encoder->usr, "bad image type\n");
    }
    return QUIC_OK;
}