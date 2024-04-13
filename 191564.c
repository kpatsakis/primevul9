void ldb_set_default_dns(struct ldb_context *ldb)
{
	TALLOC_CTX *tmp_ctx;
	int ret;
	struct ldb_result *res;
	struct ldb_dn *tmp_dn=NULL;
	static const char *attrs[] = {
		"rootDomainNamingContext",
		"configurationNamingContext",
		"schemaNamingContext",
		"defaultNamingContext",
		NULL
	};

	tmp_ctx = talloc_new(ldb);
	ret = ldb_search(ldb, tmp_ctx, &res, ldb_dn_new(tmp_ctx, ldb, NULL),
			 LDB_SCOPE_BASE, attrs, "(objectClass=*)");
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return;
	}

	if (res->count != 1) {
		talloc_free(tmp_ctx);
		return;
	}

	if (!ldb_get_opaque(ldb, "rootDomainNamingContext")) {
		tmp_dn = ldb_msg_find_attr_as_dn(ldb, ldb, res->msgs[0],
						 "rootDomainNamingContext");
		ldb_set_opaque(ldb, "rootDomainNamingContext", tmp_dn);
	}

	if (!ldb_get_opaque(ldb, "configurationNamingContext")) {
		tmp_dn = ldb_msg_find_attr_as_dn(ldb, ldb, res->msgs[0],
						 "configurationNamingContext");
		ldb_set_opaque(ldb, "configurationNamingContext", tmp_dn);
	}

	if (!ldb_get_opaque(ldb, "schemaNamingContext")) {
		tmp_dn = ldb_msg_find_attr_as_dn(ldb, ldb, res->msgs[0],
						 "schemaNamingContext");
		ldb_set_opaque(ldb, "schemaNamingContext", tmp_dn);
	}

	if (!ldb_get_opaque(ldb, "defaultNamingContext")) {
		tmp_dn = ldb_msg_find_attr_as_dn(ldb, ldb, res->msgs[0],
						 "defaultNamingContext");
		ldb_set_opaque(ldb, "defaultNamingContext", tmp_dn);
	}

	talloc_free(tmp_ctx);
}