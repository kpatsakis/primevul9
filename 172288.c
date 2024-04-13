static long long lsqlite3_get_eid(struct lsqlite3_private *lsqlite3,
				  struct ldb_dn *dn)
{
	TALLOC_CTX *local_ctx;
	long long eid = -1;
	char *cdn;

	/* ignore ltdb specials */
	if (ldb_dn_is_special(dn)) {
		return -1;
	}

	/* create a local ctx */
	local_ctx = talloc_named(lsqlite3, 0, "lsqlite3_get_eid local context");
	if (local_ctx == NULL) {
		return -1;
	}

	cdn = ldb_dn_alloc_casefold(local_ctx, dn);
	if (!cdn) goto done;

	eid = lsqlite3_get_eid_ndn(lsqlite3->sqlite, local_ctx, cdn);

done:
	talloc_free(local_ctx);
	return eid;
}