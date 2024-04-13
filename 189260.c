    const char *libraw_strerror(int e)
    {
        enum LibRaw_errors errorcode = (LibRaw_errors)e;
        switch(errorcode)
            {
            case        LIBRAW_SUCCESS:
                return "No error";
            case        LIBRAW_UNSPECIFIED_ERROR:
                return "Unspecified error";
            case        LIBRAW_FILE_UNSUPPORTED:
                return "Unsupported file format or not RAW file";
            case        LIBRAW_REQUEST_FOR_NONEXISTENT_IMAGE:
                return "Request for nonexisting image number";
            case        LIBRAW_OUT_OF_ORDER_CALL:
                return "Out of order call of libraw function";
            case    LIBRAW_NO_THUMBNAIL:
                return "No thumbnail in file";
            case    LIBRAW_UNSUPPORTED_THUMBNAIL:
                return "Unsupported thumbnail format";
            case    LIBRAW_UNSUFFICIENT_MEMORY:
                return "Unsufficient memory";
            case    LIBRAW_DATA_ERROR:
                return "Corrupted data or unexpected EOF";
            case    LIBRAW_IO_ERROR:
                return "Input/output error";
            case LIBRAW_CANCELLED_BY_CALLBACK:
                return "Cancelled by user callback";
            case LIBRAW_BAD_CROP:
                return "Bad crop box";
            default:
                return "Unknown error code";
        }
    }