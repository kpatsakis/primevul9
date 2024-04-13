function_memsize(const void *p)
{
    /* const */ffi_cif *ptr = (ffi_cif *)p;
    size_t size = 0;

    size += sizeof(*ptr);
#if !defined(FFI_NO_RAW_API) || !FFI_NO_RAW_API
    size += ffi_raw_size(ptr);
#endif

    return size;
}