PHP_FUNCTION(curl_file_create)
{
    object_init_ex( return_value, curl_CURLFile_class );
    curlfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}