static inline UBool objectEquals(const UObject* a, const UObject* b) {
    // LATER: return *a == *b;
    return *((const Measure*) a) == *((const Measure*) b);
}