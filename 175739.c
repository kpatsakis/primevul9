static int uint64_mul_overflow(uint64_t a, uint64_t b, uint64_t *res)
{
    *res = a * b;
    return (a > UINT64_MAX / b);
}