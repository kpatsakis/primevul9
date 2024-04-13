get_last_end_pos(asdl_seq *s, int *end_lineno, int *end_col_offset)
{
    Py_ssize_t tot = asdl_seq_LEN(s);
    // There must be no empty suites.
    assert(tot > 0);
    stmt_ty last = asdl_seq_GET(s, tot - 1);
    *end_lineno = last->end_lineno;
    *end_col_offset = last->end_col_offset;
}