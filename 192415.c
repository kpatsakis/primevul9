static void JSON_free(void *ptr)
{
    JSON_Parser *json = ptr;
    fbuffer_free(json->fbuffer);
    ruby_xfree(json);
}