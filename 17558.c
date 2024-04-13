static int sanity_check_int_value(struct snd_card *card,
				  const struct snd_ctl_elem_value *control,
				  const struct snd_ctl_elem_info *info,
				  int i, bool print_error)
{
	long long lval, lmin, lmax, lstep;
	u64 rem;

	switch (info->type) {
	default:
	case SNDRV_CTL_ELEM_TYPE_BOOLEAN:
		lval = control->value.integer.value[i];
		lmin = 0;
		lmax = 1;
		lstep = 0;
		break;
	case SNDRV_CTL_ELEM_TYPE_INTEGER:
		lval = control->value.integer.value[i];
		lmin = info->value.integer.min;
		lmax = info->value.integer.max;
		lstep = info->value.integer.step;
		break;
	case SNDRV_CTL_ELEM_TYPE_INTEGER64:
		lval = control->value.integer64.value[i];
		lmin = info->value.integer64.min;
		lmax = info->value.integer64.max;
		lstep = info->value.integer64.step;
		break;
	case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
		lval = control->value.enumerated.item[i];
		lmin = 0;
		lmax = info->value.enumerated.items - 1;
		lstep = 0;
		break;
	}

	if (lval < lmin || lval > lmax) {
		if (print_error)
			dev_err(card->dev,
				"control %i:%i:%i:%s:%i: value out of range %lld (%lld/%lld) at count %i\n",
				control->id.iface, control->id.device,
				control->id.subdevice, control->id.name,
				control->id.index, lval, lmin, lmax, i);
		return -EINVAL;
	}
	if (lstep) {
		div64_u64_rem(lval, lstep, &rem);
		if (rem) {
			if (print_error)
				dev_err(card->dev,
					"control %i:%i:%i:%s:%i: unaligned value %lld (step %lld) at count %i\n",
					control->id.iface, control->id.device,
					control->id.subdevice, control->id.name,
					control->id.index, lval, lstep, i);
			return -EINVAL;
		}
	}

	return 0;
}