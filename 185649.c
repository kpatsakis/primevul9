mwifiex_wmm_setup_ac_downgrade(struct mwifiex_private *priv)
{
	int ac_val;

	mwifiex_dbg(priv->adapter, INFO, "info: WMM: AC Priorities:\t"
		    "BK(0), BE(1), VI(2), VO(3)\n");

	if (!priv->wmm_enabled) {
		/* WMM is not enabled, default priorities */
		for (ac_val = WMM_AC_BK; ac_val <= WMM_AC_VO; ac_val++)
			priv->wmm.ac_down_graded_vals[ac_val] =
						(enum mwifiex_wmm_ac_e) ac_val;
	} else {
		for (ac_val = WMM_AC_BK; ac_val <= WMM_AC_VO; ac_val++) {
			priv->wmm.ac_down_graded_vals[ac_val]
				= mwifiex_wmm_eval_downgrade_ac(priv,
						(enum mwifiex_wmm_ac_e) ac_val);
			mwifiex_dbg(priv->adapter, INFO,
				    "info: WMM: AC PRIO %d maps to %d\n",
				    ac_val,
				    priv->wmm.ac_down_graded_vals[ac_val]);
		}
	}
}