static void ml_combine_effects(struct ff_effect *effect,
			       struct ml_effect_state *state,
			       int gain)
{
	struct ff_effect *new = state->effect;
	unsigned int strong, weak, i;
	int x, y;
	s16 level;

	switch (new->type) {
	case FF_CONSTANT:
		i = new->direction * 360 / 0xffff;
		level = fixp_new16(apply_envelope(state,
					new->u.constant.level,
					&new->u.constant.envelope));
		x = fixp_mult(fixp_sin16(i), level) * gain / 0xffff;
		y = fixp_mult(-fixp_cos16(i), level) * gain / 0xffff;
		/*
		 * here we abuse ff_ramp to hold x and y of constant force
		 * If in future any driver wants something else than x and y
		 * in s8, this should be changed to something more generic
		 */
		effect->u.ramp.start_level =
			clamp_val(effect->u.ramp.start_level + x, -0x80, 0x7f);
		effect->u.ramp.end_level =
			clamp_val(effect->u.ramp.end_level + y, -0x80, 0x7f);
		break;

	case FF_RUMBLE:
		strong = (u32)new->u.rumble.strong_magnitude * gain / 0xffff;
		weak = (u32)new->u.rumble.weak_magnitude * gain / 0xffff;

		if (effect->u.rumble.strong_magnitude + strong)
			effect->direction = ml_calculate_direction(
				effect->direction,
				effect->u.rumble.strong_magnitude,
				new->direction, strong);
		else if (effect->u.rumble.weak_magnitude + weak)
			effect->direction = ml_calculate_direction(
				effect->direction,
				effect->u.rumble.weak_magnitude,
				new->direction, weak);
		else
			effect->direction = 0;
		effect->u.rumble.strong_magnitude =
			min(strong + effect->u.rumble.strong_magnitude,
			    0xffffU);
		effect->u.rumble.weak_magnitude =
			min(weak + effect->u.rumble.weak_magnitude, 0xffffU);
		break;

	case FF_PERIODIC:
		i = apply_envelope(state, abs(new->u.periodic.magnitude),
				   &new->u.periodic.envelope);

		/* here we also scale it 0x7fff => 0xffff */
		i = i * gain / 0x7fff;

		if (effect->u.rumble.strong_magnitude + i)
			effect->direction = ml_calculate_direction(
				effect->direction,
				effect->u.rumble.strong_magnitude,
				new->direction, i);
		else
			effect->direction = 0;
		effect->u.rumble.strong_magnitude =
			min(i + effect->u.rumble.strong_magnitude, 0xffffU);
		effect->u.rumble.weak_magnitude =
			min(i + effect->u.rumble.weak_magnitude, 0xffffU);
		break;

	default:
		pr_err("invalid type in ml_combine_effects()\n");
		break;
	}

}