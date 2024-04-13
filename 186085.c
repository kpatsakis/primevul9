static void of_unittest_untrack_overlay(int id)
{
	if (overlay_first_id < 0)
		return;
	id -= overlay_first_id;
	BUG_ON(id >= MAX_UNITTEST_OVERLAYS);
	overlay_id_bits[BIT_WORD(id)] &= ~BIT_MASK(id);
}