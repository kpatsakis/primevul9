static void bond_skip_slave(struct bond_up_slave *slaves,
			    struct slave *skipslave)
{
	int idx;

	/* Rare situation where caller has asked to skip a specific
	 * slave but allocation failed (most likely!). BTW this is
	 * only possible when the call is initiated from
	 * __bond_release_one(). In this situation; overwrite the
	 * skipslave entry in the array with the last entry from the
	 * array to avoid a situation where the xmit path may choose
	 * this to-be-skipped slave to send a packet out.
	 */
	for (idx = 0; slaves && idx < slaves->count; idx++) {
		if (skipslave == slaves->arr[idx]) {
			slaves->arr[idx] =
				slaves->arr[slaves->count - 1];
			slaves->count--;
			break;
		}
	}
}