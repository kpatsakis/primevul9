static inline UObject* objectClone(const UObject* a) {
    // LATER: return a->clone();
    return ((const Measure*) a)->clone();
}