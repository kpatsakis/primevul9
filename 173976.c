static int ceil_log_2(int val) /* ceil(log_2(val)) */
{
    int result;

    //spice_assert(val>0);

    if (val == 1) {
        return 0;
    }

    result = 1;
    val -= 1;
    while (val >>= 1) {
        result++;
    }

    return result;
}