htmlSaveErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_OUTPUT, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}