static inline void init_decode_io(Encoder *encoder)
{
    encoder->io_next_word = encoder->io_word = GUINT32_FROM_LE(*(encoder->io_now));
    encoder->io_now++;
    encoder->io_available_bits = 0;
}