static int listOpDescriptions(void *_core, const char *k, const char *v) {
        r_cons_printf ("%s=%s\n", k, v);
        return 1;
}