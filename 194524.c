j2ku_shift(unsigned x, int n) {
    if (n < 0) {
        return x >> -n;
    } else {
        return x << n;
    }
}