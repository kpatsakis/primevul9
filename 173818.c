static void lg4ff_init_wheel_data(struct lg4ff_wheel_data * const wdata, const struct lg4ff_wheel *wheel,
				  const struct lg4ff_multimode_wheel *mmode_wheel,
				  const u16 real_product_id)
{
	u32 alternate_modes = 0;
	const char *real_tag = NULL;
	const char *real_name = NULL;

	if (mmode_wheel) {
		alternate_modes = mmode_wheel->alternate_modes;
		real_tag = mmode_wheel->real_tag;
		real_name = mmode_wheel->real_name;
	}

	{
		struct lg4ff_wheel_data t_wdata =  { .product_id = wheel->product_id,
						     .real_product_id = real_product_id,
						     .combine = 0,
						     .min_range = wheel->min_range,
						     .max_range = wheel->max_range,
						     .set_range = wheel->set_range,
						     .alternate_modes = alternate_modes,
						     .real_tag = real_tag,
						     .real_name = real_name };

		memcpy(wdata, &t_wdata, sizeof(t_wdata));
	}
}