static int ql_is_link_full_dup(struct ql3_adapter *qdev)
{
	if (ql_is_fiber(qdev))
		return 1;
	else
		return ql_is_full_dup(qdev);
}