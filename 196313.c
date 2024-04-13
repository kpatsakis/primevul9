void DecimalQuantity::ensureCapacity(int32_t capacity) {
    if (capacity == 0) { return; }
    int32_t oldCapacity = usingBytes ? fBCD.bcdBytes.len : 0;
    if (!usingBytes) {
        // TODO: There is nothing being done to check for memory allocation failures.
        // TODO: Consider indexing by nybbles instead of bytes in C++, so that we can
        // make these arrays half the size.
        fBCD.bcdBytes.ptr = static_cast<int8_t*>(uprv_malloc(capacity * sizeof(int8_t)));
        fBCD.bcdBytes.len = capacity;
        // Initialize the byte array to zeros (this is done automatically in Java)
        uprv_memset(fBCD.bcdBytes.ptr, 0, capacity * sizeof(int8_t));
    } else if (oldCapacity < capacity) {
        auto bcd1 = static_cast<int8_t*>(uprv_malloc(capacity * 2 * sizeof(int8_t)));
        uprv_memcpy(bcd1, fBCD.bcdBytes.ptr, oldCapacity * sizeof(int8_t));
        // Initialize the rest of the byte array to zeros (this is done automatically in Java)
        uprv_memset(bcd1 + oldCapacity, 0, (capacity - oldCapacity) * sizeof(int8_t));
        uprv_free(fBCD.bcdBytes.ptr);
        fBCD.bcdBytes.ptr = bcd1;
        fBCD.bcdBytes.len = capacity * 2;
    }
    usingBytes = true;
}