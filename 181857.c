dup_identity_check(const char *id, struct lys_ident *ident, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++) {
        if (ly_strequal(id, ident[i].name, 1)) {
            /* name collision */
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}