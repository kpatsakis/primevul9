static inline void read_io_word(Encoder *encoder)
{
    if (G_UNLIKELY(encoder->io_now == encoder->io_end)) {
        more_io_words(encoder);
    }
    spice_extra_assert(encoder->io_now < encoder->io_end);

    encoder->io_next_word = GUINT32_FROM_LE(*(encoder->io_now));
    encoder->io_now++;
}