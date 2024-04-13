static uint16_t ip_checksum(void *data, size_t len)
{
    return ~ones_complement_sum((uint8_t*)data, len);
}