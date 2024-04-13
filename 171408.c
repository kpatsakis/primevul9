static void ql_init_scan_mode(struct ql3_adapter *qdev)
{
	ql_mii_enable_scan_mode(qdev);

	if (test_bit(QL_LINK_OPTICAL, &qdev->flags)) {
		if (ql_this_adapter_controls_port(qdev))
			ql_petbi_init_ex(qdev);
	} else {
		if (ql_this_adapter_controls_port(qdev))
			ql_phy_init_ex(qdev);
	}
}