static inline void decode_eatbits(Encoder *encoder, int len)
{
    int delta;

    spice_extra_assert(len > 0 && len < 32);
    encoder->io_word <<= len;

    if ((delta = ((int)encoder->io_available_bits - len)) >= 0) {
        encoder->io_available_bits = delta;
        encoder->io_word |= encoder->io_next_word >> encoder->io_available_bits;
        return;
    }

    delta = -delta;
    encoder->io_word |= encoder->io_next_word << delta;
    read_io_word(encoder);
    encoder->io_available_bits = 32 - delta;
    encoder->io_word |= (encoder->io_next_word >> encoder->io_available_bits);
}