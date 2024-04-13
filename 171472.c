static void ql_petbi_init_ex(struct ql3_adapter *qdev)
{
	ql_petbi_reset_ex(qdev);
	ql_petbi_start_neg_ex(qdev);
}