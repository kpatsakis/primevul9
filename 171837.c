static void bcon_json_print(bcon *bc, int n) {
    int t = 0;
    int key_value_count = 0;
    char *s;
    int end_of_data;
    bcon *bcp;
    putchar('{');
    for (end_of_data = 0, bcp = bc; !end_of_data; bcp++) {
        bcon bci = *bcp;
        switch (t) {
        case 'l':
            if (key_value_count & 0x1) putchar(':');
            printf("%ld", bci.l);
            t = 0;
            key_value_count++;
            break;
        case 's': /* fall through */
        default:
            s = bci.s;
            switch (*s) {
            case ':':
                ++s;
                t = *++s;
                break;
            case '{':
                if (key_value_count & 0x1) putchar(':');
                putchar(*s);
                key_value_count = 0;
                break;
            case '}':
                putchar(*s);
                key_value_count = 2;
                break;
            default:
                if (key_value_count & 0x1) putchar(':');
                else if (key_value_count > 1) putchar(',');
                printf("\"%s\"", s);
                t = 0;
                key_value_count++;
                break;
            }
            break;
        }
    }
    putchar('}');
}