yang_free_ident_base(struct lys_ident *ident, uint32_t start, uint32_t size)
{
    uint32_t i;
    uint8_t j;

    /* free base name */
    for (i = start; i < size; ++i) {
        for (j = 0; j < ident[i].base_size; ++j) {
            free(ident[i].base[j]);
        }
    }
}