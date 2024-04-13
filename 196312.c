static inline UBool instanceOfMeasure(const UObject* a) {
    return dynamic_cast<const Measure*>(a) != NULL;
}