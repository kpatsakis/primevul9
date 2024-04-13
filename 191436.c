MagickExport size_t GetMaxMemoryRequest(void)
{
#define MinMemoryRequest "16MiB"

  if (max_memory_request == 0)
    {
      char
        *value;

      max_memory_request=(size_t) MagickULLConstant(~0);
      value=GetPolicyValue("system:max-memory-request");
      if (value != (char *) NULL)
        {
          /*
            The security policy sets a max memory request limit.
          */
          max_memory_request=MagickMax(StringToSizeType(value,100.0),
            StringToSizeType(MinMemoryRequest,100.0));
          value=DestroyString(value);
        }
    }
  return(MagickMin(max_memory_request,MAGICK_SSIZE_MAX));
}