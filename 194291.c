static const struct ff_envelope *get_envelope(const struct ff_effect *effect)
{
	static const struct ff_envelope empty_envelope;

	switch (effect->type) {
	case FF_PERIODIC:
		return &effect->u.periodic.envelope;

	case FF_CONSTANT:
		return &effect->u.constant.envelope;

	default:
		return &empty_envelope;
	}
}