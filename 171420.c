static int ql_is_neg_pause(struct ql3_adapter *qdev)
{
	if (ql_is_fiber(qdev))
		return ql_is_petbi_neg_pause(qdev);
	else
		return ql_is_phy_neg_pause(qdev);
}