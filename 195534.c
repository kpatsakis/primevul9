ast_for_with_stmt(struct compiling *c, const node *n0, bool is_async)
{
    const node * const n = is_async ? CHILD(n0, 1) : n0;
    int i, n_items, nch_minus_type, has_type_comment, end_lineno, end_col_offset;
    asdl_seq *items, *body;
    string type_comment;

    if (is_async && c->c_feature_version < 5) {
        ast_error(c, n,
                  "Async with statements are only supported in Python 3.5 and greater");
        return NULL;
    }

    REQ(n, with_stmt);

    has_type_comment = TYPE(CHILD(n, NCH(n) - 2)) == TYPE_COMMENT;
    nch_minus_type = NCH(n) - has_type_comment;

    n_items = (nch_minus_type - 2) / 2;
    items = _Py_asdl_seq_new(n_items, c->c_arena);
    if (!items)
        return NULL;
    for (i = 1; i < nch_minus_type - 2; i += 2) {
        withitem_ty item = ast_for_with_item(c, CHILD(n, i));
        if (!item)
            return NULL;
        asdl_seq_SET(items, (i - 1) / 2, item);
    }

    body = ast_for_suite(c, CHILD(n, NCH(n) - 1));
    if (!body)
        return NULL;
    get_last_end_pos(body, &end_lineno, &end_col_offset);

    if (has_type_comment) {
        type_comment = NEW_TYPE_COMMENT(CHILD(n, NCH(n) - 2));
        if (!type_comment)
            return NULL;
    }
    else
        type_comment = NULL;

    if (is_async)
        return AsyncWith(items, body, type_comment, LINENO(n0), n0->n_col_offset,
                         end_lineno, end_col_offset, c->c_arena);
    else
        return With(items, body, type_comment, LINENO(n), n->n_col_offset,
                    end_lineno, end_col_offset, c->c_arena);
}