static inline int sony_compare_connection_type(struct sony_sc *sc0,
						struct sony_sc *sc1)
{
	const int sc0_not_bt = !(sc0->quirks & SONY_BT_DEVICE);
	const int sc1_not_bt = !(sc1->quirks & SONY_BT_DEVICE);

	return sc0_not_bt == sc1_not_bt;
}