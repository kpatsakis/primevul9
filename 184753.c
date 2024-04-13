static int nfp_abm_update_band_map(struct nfp_abm_link *alink)
{
	unsigned int i, bits_per_prio, prios_per_word, base_shift;
	struct nfp_abm *abm = alink->abm;
	u32 field_mask;

	alink->has_prio = !list_empty(&alink->dscp_map);

	bits_per_prio = roundup_pow_of_two(order_base_2(abm->num_bands));
	field_mask = (1 << bits_per_prio) - 1;
	prios_per_word = sizeof(u32) * BITS_PER_BYTE / bits_per_prio;

	/* FW mask applies from top bits */
	base_shift = 8 - order_base_2(abm->num_prios);

	for (i = 0; i < abm->num_prios; i++) {
		unsigned int offset;
		u32 *word;
		u8 band;

		word = &alink->prio_map[i / prios_per_word];
		offset = (i % prios_per_word) * bits_per_prio;

		band = nfp_abm_find_band_for_prio(alink, i << base_shift);

		*word &= ~(field_mask << offset);
		*word |= band << offset;
	}

	/* Qdisc offload status may change if has_prio changed */
	nfp_abm_qdisc_offload_update(alink);

	return nfp_abm_ctrl_prio_map_update(alink, alink->prio_map);
}