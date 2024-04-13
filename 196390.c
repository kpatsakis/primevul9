static Formattable* createArrayCopy(const Formattable* array, int32_t count) {
    Formattable *result = new Formattable[count];
    if (result != NULL) {
        for (int32_t i=0; i<count; ++i)
            result[i] = array[i]; // Don't memcpy!
    }
    return result;
}