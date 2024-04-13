static void ml_play_effects(struct ml_device *ml)
{
	struct ff_effect effect;
	DECLARE_BITMAP(handled_bm, FF_MEMLESS_EFFECTS);

	memset(handled_bm, 0, sizeof(handled_bm));

	while (ml_get_combo_effect(ml, handled_bm, &effect))
		ml->play_effect(ml->dev, ml->private, &effect);

	ml_schedule_timer(ml);
}