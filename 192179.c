static void free_eops(extop *e)
{
    extop *next;

    while (e) {
        next = e->next;
        switch (e->type) {
        case EOT_EXTOP:
            free_eops(e->val.subexpr);
            break;

        case EOT_DB_STRING_FREE:
            nasm_free(e->val.string.data);
            break;

        default:
            break;
        }

        nasm_free(e);
        e = next;
    }
}