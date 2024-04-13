static const char *qxl_mode_to_string(int mode)
{
    switch (mode) {
    case QXL_MODE_COMPAT:
        return "compat";
    case QXL_MODE_NATIVE:
        return "native";
    case QXL_MODE_UNDEFINED:
        return "undefined";
    case QXL_MODE_VGA:
        return "vga";
    }
    return "INVALID";
}