static void alloc_file_image(MemBuffer &mb, off_t size)
{
    assert(mem_size_valid_bytes(size));
    if (mb.getVoidPtr() == nullptr) {
        mb.alloc(size);
    } else {
        assert((u32_t)size <= mb.getSize());
    }
}