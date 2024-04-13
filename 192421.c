static size_t JSON_memsize(const void *ptr)
{
    const JSON_Parser *json = ptr;
    return sizeof(*json) + FBUFFER_CAPA(json->fbuffer);
}