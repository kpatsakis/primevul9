wc_push_to_iso2022_end(Str os, wc_status *st)
{
    if (st->design[1] != 0 && st->design[1] != st->g1_ccs)
	wc_push_iso2022_esc(os, st->g1_ccs, WC_C_G1_CS94, 0, st);
    wc_push_iso2022_esc(os, st->g0_ccs, WC_C_G0_CS94, 1, st);
}