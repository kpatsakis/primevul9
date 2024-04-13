static inline void encode(Encoder *encoder, unsigned int word, unsigned int len)
{
    int delta;

    spice_extra_assert(len > 0 && len < 32);
    spice_extra_assert(!(word & ~bppmask[len]));

    if ((delta = ((int)encoder->io_available_bits - len)) >= 0) {
        encoder->io_available_bits = delta;
        encoder->io_word |= word << encoder->io_available_bits;
        return;
    }
    delta = -delta;
    encoder->io_word |= word >> delta;
    write_io_word(encoder);
    encoder->io_available_bits = 32 - delta;
    encoder->io_word = word << encoder->io_available_bits;

    spice_extra_assert(encoder->io_available_bits < 32);
    spice_extra_assert((encoder->io_word & bppmask[encoder->io_available_bits]) == 0);
}