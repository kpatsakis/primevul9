static void more_io_words(Encoder *encoder)
{
    uint32_t *io_ptr;
    int num_io_words = encoder->usr->more_space(encoder->usr, &io_ptr, encoder->rows_completed);
    if (num_io_words <= 0) {
        encoder->usr->error(encoder->usr, "%s: no more words\n", __FUNCTION__);
    }
    spice_assert(io_ptr);
    encoder->io_words_count += num_io_words;
    encoder->io_now = io_ptr;
    encoder->io_end = encoder->io_now + num_io_words;
}