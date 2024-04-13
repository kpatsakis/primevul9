combine(const char *val1, const char *val2, char **val_out)
{
    if (val1 == NULL && val2 == NULL) {
        *val_out = NULL;
    } else if (val1 != NULL && val2 != NULL) {
        if (asprintf(val_out, "%s %s", val1, val2) < 0) {
            *val_out = NULL;
            return ENOMEM;
        }
    } else {
        *val_out = strdup((val1 != NULL) ? val1 : val2);
        if (*val_out == NULL)
            return ENOMEM;
    }
    return 0;
}