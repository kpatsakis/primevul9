static uint32_t qpp_hash(QppEntry e)
{
    return qemu_xxhash7(e.ino_prefix, e.dev, 0, 0, 0);
}