static inline void write_io_word(Encoder *encoder)
{
    if (G_UNLIKELY(encoder->io_now == encoder->io_end)) {
        more_io_words(encoder);
    }
    *(encoder->io_now++) = GUINT32_TO_LE(encoder->io_word);
}