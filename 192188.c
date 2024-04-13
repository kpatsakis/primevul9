const expr *next_expr(const expr *e, const expr **next_list)
{
    e++;
    if (!e->type) {
        if (next_list) {
            e = *next_list;
            *next_list = NULL;
        } else {
            e = NULL;
        }
    }
    return e;
}