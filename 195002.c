static uint16_t ones_complement_sum(uint8_t *data, size_t len)
{
    uint32_t result = 0;

    for (; len > 1; data+=2, len-=2)
    {
        result += *(uint16_t*)data;
    }

    /* add the remainder byte */
    if (len)
    {
        uint8_t odd[2] = {*data, 0};
        result += *(uint16_t*)odd;
    }

    while (result>>16)
        result = (result & 0xffff) + (result >> 16);

    return result;
}