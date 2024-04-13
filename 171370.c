
static u32 ql_supported_modes(struct ql3_adapter *qdev)
{
	if (test_bit(QL_LINK_OPTICAL, &qdev->flags))
		return SUPPORTED_OPTICAL_MODES;

	return SUPPORTED_TP_MODES;