u8 mwifiex_wmm_downgrade_tid(struct mwifiex_private *priv, u32 tid)
{
	enum mwifiex_wmm_ac_e ac, ac_down;
	u8 new_tid;

	ac = mwifiex_wmm_convert_tos_to_ac(priv->adapter, tid);
	ac_down = priv->wmm.ac_down_graded_vals[ac];

	/* Send the index to tid array, picking from the array will be
	 * taken care by dequeuing function
	 */
	new_tid = ac_to_tid[ac_down][tid % 2];

	return new_tid;
}