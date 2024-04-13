static void decode_mntname(char *name, int len)
{
    int i, j = 0;
    for (i = 0; i <= len; i++) {
        if (name[i] != '\\') {
            name[j++] = name[i];
        } else if (name[i + 1] == '\\') {
            name[j++] = '\\';
            i++;
        } else if (name[i + 1] >= '0' && name[i + 1] <= '3' &&
                   name[i + 2] >= '0' && name[i + 2] <= '7' &&
                   name[i + 3] >= '0' && name[i + 3] <= '7') {
            name[j++] = (name[i + 1] - '0') * 64 +
                        (name[i + 2] - '0') * 8 +
                        (name[i + 3] - '0');
            i += 3;
        } else {
            name[j++] = name[i];
        }
    }
}