static void print_sg(struct iovec *sg, int cnt)
{
    int i;

    printf("sg[%d]: {", cnt);
    for (i = 0; i < cnt; i++) {
        if (i) {
            printf(", ");
        }
        printf("(%p, %zd)", sg[i].iov_base, sg[i].iov_len);
    }
    printf("}\n");
}