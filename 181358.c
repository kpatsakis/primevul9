static inline bool is_read_only_op(V9fsPDU *pdu)
{
    switch (pdu->id) {
    case P9_TREADDIR:
    case P9_TSTATFS:
    case P9_TGETATTR:
    case P9_TXATTRWALK:
    case P9_TLOCK:
    case P9_TGETLOCK:
    case P9_TREADLINK:
    case P9_TVERSION:
    case P9_TLOPEN:
    case P9_TATTACH:
    case P9_TSTAT:
    case P9_TWALK:
    case P9_TCLUNK:
    case P9_TFSYNC:
    case P9_TOPEN:
    case P9_TREAD:
    case P9_TAUTH:
    case P9_TFLUSH:
        return 1;
    default:
        return 0;
    }
}