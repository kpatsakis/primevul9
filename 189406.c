display_usage(int (*output)(const char *))
{
    struct sudo_lbuf lbuf;
    char *uvec[6];
    int i, ulen;

    /*
     * Use usage vectors appropriate to the progname.
     */
    if (strcmp(getprogname(), "sudoedit") == 0) {
	uvec[0] = &SUDO_USAGE5[3];
	uvec[1] = NULL;
    } else {
	uvec[0] = SUDO_USAGE1;
	uvec[1] = SUDO_USAGE2;
	uvec[2] = SUDO_USAGE3;
	uvec[3] = SUDO_USAGE4;
	uvec[4] = SUDO_USAGE5;
	uvec[5] = NULL;
    }

    /*
     * Print usage and wrap lines as needed, depending on the
     * tty width.
     */
    ulen = (int)strlen(getprogname()) + 8;
    sudo_lbuf_init(&lbuf, output, ulen, NULL,
	user_details.ts_cols);
    for (i = 0; uvec[i] != NULL; i++) {
	sudo_lbuf_append(&lbuf, "usage: %s%s", getprogname(), uvec[i]);
	sudo_lbuf_print(&lbuf);
    }
    sudo_lbuf_destroy(&lbuf);
}