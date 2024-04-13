static inline void flush(Encoder *encoder)
{
    if (encoder->io_available_bits > 0 && encoder->io_available_bits != 32) {
        encode(encoder, 0, encoder->io_available_bits);
    }
    encode_32(encoder, 0);
    encode(encoder, 0, 1);
}