static int prefix_slot(int prefix)
{
    switch (prefix) {
    case P_WAIT:
        return PPS_WAIT;
    case R_CS:
    case R_DS:
    case R_SS:
    case R_ES:
    case R_FS:
    case R_GS:
        return PPS_SEG;
    case P_LOCK:
        return PPS_LOCK;
    case P_REP:
    case P_REPE:
    case P_REPZ:
    case P_REPNE:
    case P_REPNZ:
    case P_XACQUIRE:
    case P_XRELEASE:
    case P_BND:
    case P_NOBND:
        return PPS_REP;
    case P_O16:
    case P_O32:
    case P_O64:
    case P_OSP:
        return PPS_OSIZE;
    case P_A16:
    case P_A32:
    case P_A64:
    case P_ASP:
        return PPS_ASIZE;
    case P_EVEX:
    case P_VEX3:
    case P_VEX2:
        return PPS_VEX;
    default:
        nasm_panic("Invalid value %d passed to prefix_slot()", prefix);
        return -1;
    }
}