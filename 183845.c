yang_fill_ext_substm_index(struct lys_ext_instance_complex *ext, LY_STMT stmt, enum yytokentype keyword)
{
    int c = 0, decrement = 0;
    const char **str, ***p = NULL;
    struct lyext_substmt *info;


    if (keyword == BELONGS_TO_KEYWORD || stmt == LY_STMT_BELONGSTO) {
        stmt = LY_STMT_BELONGSTO;
        decrement = -1;
    } else if (keyword == ARGUMENT_KEYWORD || stmt == LY_STMT_ARGUMENT) {
        stmt = LY_STMT_ARGUMENT;
        decrement = -1;
    }

    str = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!str || info->cardinality < LY_STMT_CARD_SOME || !((const char ***)str)[0]) {
        return 0;
    } else {
        p = (const char ***)str;
        /* get the index in the array */
        for (c = 0; p[0][c]; c++);
        return c + decrement;
    }
}