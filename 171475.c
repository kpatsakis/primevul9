
static void ql_tx_timeout_work(struct work_struct *work)
{
	struct ql3_adapter *qdev =
		container_of(work, struct ql3_adapter, tx_timeout_work.work);

	ql_cycle_adapter(qdev, QL_DO_RESET);