static void quic_image_params(Encoder *encoder, QuicImageType type, int *channels, int *bpc)
{
    spice_assert(channels && bpc);
    switch (type) {
    case QUIC_IMAGE_TYPE_GRAY:
        *channels = 1;
        *bpc = 8;
        break;
    case QUIC_IMAGE_TYPE_RGB16:
        *channels = 3;
        *bpc = 5;
        break;
    case QUIC_IMAGE_TYPE_RGB24:
        *channels = 3;
        *bpc = 8;
        break;
    case QUIC_IMAGE_TYPE_RGB32:
        *channels = 3;
        *bpc = 8;
        break;
    case QUIC_IMAGE_TYPE_RGBA:
        *channels = 4;
        *bpc = 8;
        break;
    case QUIC_IMAGE_TYPE_INVALID:
    default:
        *channels = 0;
        *bpc = 0;
        encoder->usr->error(encoder->usr, "bad image type\n");
    }
}