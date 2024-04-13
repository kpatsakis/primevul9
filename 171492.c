static void ql_petbi_init(struct ql3_adapter *qdev)
{
	ql_petbi_reset(qdev);
	ql_petbi_start_neg(qdev);
}