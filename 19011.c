dnsc_shared_secrets_deldatafunc(void* d, void* ATTR_UNUSED(arg))
{
    uint8_t* data = (uint8_t*)d;
    free(data);
}