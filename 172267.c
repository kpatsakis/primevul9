static void lsqlite3_compare(sqlite3_context *ctx, int argc,
					sqlite3_value **argv)
{
	struct ldb_context *ldb = (struct ldb_context *)sqlite3_user_data(ctx);
	const char *val = (const char *)sqlite3_value_text(argv[0]);
	const char *func = (const char *)sqlite3_value_text(argv[1]);
	const char *cmp = (const char *)sqlite3_value_text(argv[2]);
	const char *attr = (const char *)sqlite3_value_text(argv[3]);
	const struct ldb_schema_attribute *a;
	struct ldb_val valX;
	struct ldb_val valY;
	int ret;

	switch (func[0]) {
	/* greater */
	case '>': /* >= */
		a = ldb_schema_attribute_by_name(ldb, attr);
		valX.data = (uint8_t *)cmp;
		valX.length = strlen(cmp);
		valY.data = (uint8_t *)val;
		valY.length = strlen(val);
		ret = a->syntax->comparison_fn(ldb, ldb, &valY, &valX);
		if (ret >= 0)
			sqlite3_result_int(ctx, 1);
		else
			sqlite3_result_int(ctx, 0);
		return;

	/* lesser */
	case '<': /* <= */
		a = ldb_schema_attribute_by_name(ldb, attr);
		valX.data = (uint8_t *)cmp;
		valX.length = strlen(cmp);
		valY.data = (uint8_t *)val;
		valY.length = strlen(val);
		ret = a->syntax->comparison_fn(ldb, ldb, &valY, &valX);
		if (ret <= 0)
			sqlite3_result_int(ctx, 1);
		else
			sqlite3_result_int(ctx, 0);
		return;

	/* approx */
	case '~':
		/* TODO */
		sqlite3_result_int(ctx, 0);
		return;

	/* bitops */
	case ':':
		/* TODO */
		sqlite3_result_int(ctx, 0);
		return;

	default:
		break;
	}

	sqlite3_result_error(ctx, "Value must start with a special operation char (<>~:)!", -1);
	return;
}