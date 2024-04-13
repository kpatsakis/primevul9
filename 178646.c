static unsigned short de_get16(void *ptr, guint endian)
{
       unsigned short val;

       memcpy(&val, ptr, sizeof(val));
       val = DE_ENDIAN16(val);

       return val;
}