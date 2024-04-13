void NumberFormatTest::TestDoubleLimit11439() {
    char  buf[50];
    for (int64_t num = MAX_INT64_IN_DOUBLE-10; num<=MAX_INT64_IN_DOUBLE; num++) {
        sprintf(buf, "%lld", (long long)num);
        double fNum = 0.0;
        sscanf(buf, "%lf", &fNum);
        int64_t rtNum = static_cast<int64_t>(fNum);
        if (num != rtNum) {
            errln("%s:%d MAX_INT64_IN_DOUBLE test, %lld did not round trip. Got %lld", __FILE__, __LINE__, (long long)num, (long long)rtNum);
            return;
        }
    }
    for (int64_t num = -MAX_INT64_IN_DOUBLE+10; num>=-MAX_INT64_IN_DOUBLE; num--) {
        sprintf(buf, "%lld", (long long)num);
        double fNum = 0.0;
        sscanf(buf, "%lf", &fNum);
        int64_t rtNum = static_cast<int64_t>(fNum);
        if (num != rtNum) {
            errln("%s:%d MAX_INT64_IN_DOUBLE test, %lld did not round trip. Got %lld", __FILE__, __LINE__, (long long)num, (long long)rtNum);
            return;
        }
    }
}