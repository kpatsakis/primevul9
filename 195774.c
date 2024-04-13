static inline int64_t ptr_compose(int32_t id, uint32_t offset)
{
    uint64_t i = id;
    return (i << 32) | offset;
}