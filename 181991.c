int button_set_seat(Button *b, const char *sn) {
        char *s;

        assert(b);
        assert(sn);

        s = strdup(sn);
        if (!s)
                return -ENOMEM;

        free(b->seat);
        b->seat = s;

        return 0;
}