static inline u64 intel_get_tsx_transaction(u64 tsx_tuning, u64 ax)
{
	u64 txn = (tsx_tuning & PEBS_HSW_TSX_FLAGS) >> 32;

	/* For RTM XABORTs also log the abort code from AX */
	if ((txn & PERF_TXN_TRANSACTION) && (ax & 1))
		txn |= ((ax >> 24) & 0xff) << PERF_TXN_ABORT_SHIFT;
	return txn;
}