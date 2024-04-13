static unsigned int de_get32(void *ptr, guint endian)
{
       unsigned int val;

       memcpy(&val, ptr, sizeof(val));
       val = DE_ENDIAN32(val);

       return val;
}