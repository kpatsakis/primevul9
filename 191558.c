int ldb_sequence_number(struct ldb_context *ldb,
			enum ldb_sequence_type type, uint64_t *seq_num)
{
	struct ldb_seqnum_request *seq;
	struct ldb_seqnum_result *seqr;
	struct ldb_result *res;
	TALLOC_CTX *tmp_ctx;
	int ret;

	*seq_num = 0;

	tmp_ctx = talloc_zero(ldb, struct ldb_request);
	if (tmp_ctx == NULL) {
		ldb_set_errstring(ldb, "Out of Memory");
		return LDB_ERR_OPERATIONS_ERROR;
	}
	seq = talloc_zero(tmp_ctx, struct ldb_seqnum_request);
	if (seq == NULL) {
		ldb_set_errstring(ldb, "Out of Memory");
		ret = LDB_ERR_OPERATIONS_ERROR;
		goto done;
	}
	seq->type = type;

	ret = ldb_extended(ldb, LDB_EXTENDED_SEQUENCE_NUMBER, seq, &res);
	if (ret != LDB_SUCCESS) {
		goto done;
	}
	talloc_steal(tmp_ctx, res);

	if (strcmp(LDB_EXTENDED_SEQUENCE_NUMBER, res->extended->oid) != 0) {
		ldb_set_errstring(ldb, "Invalid OID in reply");
		ret = LDB_ERR_OPERATIONS_ERROR;
		goto done;
	}
	seqr = talloc_get_type(res->extended->data,
				struct ldb_seqnum_result);
	*seq_num = seqr->seq_num;

done:
	talloc_free(tmp_ctx);
	return ret;
}