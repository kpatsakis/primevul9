const char *param2text(int res)
{
  ParameterError error = (ParameterError)res;
  switch(error) {
  case PARAM_GOT_EXTRA_PARAMETER:
    return "had unsupported trailing garbage";
  case PARAM_OPTION_UNKNOWN:
    return "is unknown";
  case PARAM_OPTION_AMBIGUOUS:
    return "is ambiguous";
  case PARAM_REQUIRES_PARAMETER:
    return "requires parameter";
  case PARAM_BAD_USE:
    return "is badly used here";
  case PARAM_BAD_NUMERIC:
    return "expected a proper numerical parameter";
  case PARAM_NEGATIVE_NUMERIC:
    return "expected a positive numerical parameter";
  case PARAM_LIBCURL_DOESNT_SUPPORT:
    return "the installed libcurl version doesn't support this";
  case PARAM_LIBCURL_UNSUPPORTED_PROTOCOL:
    return "a specified protocol is unsupported by libcurl";
  case PARAM_NO_MEM:
    return "out of memory";
  default:
    return "unknown error";
  }
}