static void of_unittest_destroy_tracked_overlays(void)
{
	int id, ret, defers, ovcs_id;

	if (overlay_first_id < 0)
		return;

	/* try until no defers */
	do {
		defers = 0;
		/* remove in reverse order */
		for (id = MAX_UNITTEST_OVERLAYS - 1; id >= 0; id--) {
			if (!(overlay_id_bits[BIT_WORD(id)] & BIT_MASK(id)))
				continue;

			ovcs_id = id + overlay_first_id;
			ret = of_overlay_remove(&ovcs_id);
			if (ret == -ENODEV) {
				pr_warn("%s: no overlay to destroy for #%d\n",
					__func__, id + overlay_first_id);
				continue;
			}
			if (ret != 0) {
				defers++;
				pr_warn("%s: overlay destroy failed for #%d\n",
					__func__, id + overlay_first_id);
				continue;
			}

			overlay_id_bits[BIT_WORD(id)] &= ~BIT_MASK(id);
		}
	} while (defers > 0);
}