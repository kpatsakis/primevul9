base160next_sql(sqlite3_context * hContext,
                int argc,
                sqlite3_value ** argv)
{
        int                         i;
        int                         len;
        char *             pTab;
        char *             pBase160 = strdup((const char *)sqlite3_value_text(argv[0]));
        char *             pStart = pBase160;

        /*
         * We need a minimum of four digits, and we will always get a multiple
         * of four digits.
         */
        if (pBase160 != NULL &&
            (len = strlen(pBase160)) >= 4 &&
            len % 4 == 0) {

                if (pBase160 == NULL) {

                        sqlite3_result_null(hContext);
                        return;
                }

                pBase160 += strlen(pBase160) - 1;

                /* We only carry through four digits: one level in the tree */
                for (i = 0; i < 4; i++) {

                        /* What base160 value does this digit have? */
                        pTab = strchr(base160tab, *pBase160);

                        /* Is there a carry? */
                        if (pTab < base160tab + sizeof(base160tab) - 1) {

                                /*
                                 * Nope.  Just increment this value and we're
                                 * done.
                                 */
                                *pBase160 = *++pTab;
                                break;
                        } else {

                                /*
                                 * There's a carry.  This value gets
                                 * base160tab[0], we decrement the buffer
                                 * pointer to get the next higher-order digit,
                                 * and continue in the loop.
                                 */
                                *pBase160-- = base160tab[0];
                        }
                }

                sqlite3_result_text(hContext,
                                    pStart,
                                    strlen(pStart),
                                    free);
        } else {
                sqlite3_result_value(hContext, argv[0]);
                if (pBase160 != NULL) {
                        free(pBase160);
                }
        }
}