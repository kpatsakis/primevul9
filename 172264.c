base160_sql(sqlite3_context * hContext,
            int argc,
            sqlite3_value ** argv)
{
    int             i;
    long long       val;
    char            result[5];

    val = sqlite3_value_int64(argv[0]);

    for (i = 3; i >= 0; i--) {

        result[i] = base160tab[val % 160];
        val /= 160;
    }

    result[4] = '\0';

    sqlite3_result_text(hContext, result, -1, SQLITE_TRANSIENT);
}