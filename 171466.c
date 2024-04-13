static void ql_get_phy_owner(struct ql3_adapter *qdev)
{
	if (ql_this_adapter_controls_port(qdev))
		set_bit(QL_LINK_MASTER, &qdev->flags);
	else
		clear_bit(QL_LINK_MASTER, &qdev->flags);
}