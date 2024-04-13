static int encoder_reset(Encoder *encoder, uint32_t *io_ptr, uint32_t *io_ptr_end)
{
    spice_assert(((uintptr_t)io_ptr % 4) == ((uintptr_t)io_ptr_end % 4));
    spice_assert(io_ptr <= io_ptr_end);

    encoder->rgb_state.waitcnt = 0;
    encoder->rgb_state.tabrand_seed = stabrand();
    encoder->rgb_state.wmidx = DEFwmistart;
    encoder->rgb_state.wmileft = DEFwminext;
    set_wm_trigger(&encoder->rgb_state);

    encoder_init_rle(&encoder->rgb_state);

    encoder->io_words_count = io_ptr_end - io_ptr;
    encoder->io_now = io_ptr;
    encoder->io_end = io_ptr_end;
    encoder->rows_completed = 0;

    return TRUE;
}