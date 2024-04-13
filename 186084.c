static void of_unittest_track_overlay(int id)
{
	if (overlay_first_id < 0)
		overlay_first_id = id;
	id -= overlay_first_id;

	/* we shouldn't need that many */
	BUG_ON(id >= MAX_UNITTEST_OVERLAYS);
	overlay_id_bits[BIT_WORD(id)] |= BIT_MASK(id);
}