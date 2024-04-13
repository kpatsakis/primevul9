static void set_wm_trigger(CommonState *state)
{
    unsigned int wm = state->wmidx;
    if (wm > 10) {
        wm = 10;
    }

    SPICE_VERIFY(DEFevol < 6);

    state->wm_trigger = besttrigtab[DEFevol / 2][wm];

    spice_assert(state->wm_trigger <= 2000);
    spice_assert(state->wm_trigger >= 1);
}