inline int32_t safeSubtract(int32_t a, int32_t b) {
    // Note: In C++, signed integer subtraction is undefined behavior.
    int32_t diff = static_cast<int32_t>(static_cast<uint32_t>(a) - static_cast<uint32_t>(b));
    if (b < 0 && diff < a) { return INT32_MAX; }
    if (b > 0 && diff > a) { return INT32_MIN; }
    return diff;
}