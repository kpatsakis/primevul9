static void process_size_override(insn *result, operand *op)
{
    if (tasm_compatible_mode) {
        switch (tokval.t_integer) {
            /* For TASM compatibility a size override inside the
             * brackets changes the size of the operand, not the
             * address type of the operand as it does in standard
             * NASM syntax. Hence:
             *
             *  mov     eax,[DWORD val]
             *
             * is valid syntax in TASM compatibility mode. Note that
             * you lose the ability to override the default address
             * type for the instruction, but we never use anything
             * but 32-bit flat model addressing in our code.
             */
        case S_BYTE:
            op->type |= BITS8;
            break;
        case S_WORD:
            op->type |= BITS16;
            break;
        case S_DWORD:
        case S_LONG:
            op->type |= BITS32;
            break;
        case S_QWORD:
            op->type |= BITS64;
            break;
        case S_TWORD:
            op->type |= BITS80;
            break;
        case S_OWORD:
            op->type |= BITS128;
            break;
        default:
            nasm_nonfatal("invalid operand size specification");
            break;
        }
    } else {
        /* Standard NASM compatible syntax */
        switch (tokval.t_integer) {
        case S_NOSPLIT:
            op->eaflags |= EAF_TIMESTWO;
            break;
        case S_REL:
            op->eaflags |= EAF_REL;
            break;
        case S_ABS:
            op->eaflags |= EAF_ABS;
            break;
        case S_BYTE:
            op->disp_size = 8;
            op->eaflags |= EAF_BYTEOFFS;
            break;
        case P_A16:
        case P_A32:
        case P_A64:
            if (result->prefixes[PPS_ASIZE] &&
                result->prefixes[PPS_ASIZE] != tokval.t_integer)
                nasm_nonfatal("conflicting address size specifications");
            else
                result->prefixes[PPS_ASIZE] = tokval.t_integer;
            break;
        case S_WORD:
            op->disp_size = 16;
            op->eaflags |= EAF_WORDOFFS;
            break;
        case S_DWORD:
        case S_LONG:
            op->disp_size = 32;
            op->eaflags |= EAF_WORDOFFS;
            break;
        case S_QWORD:
            op->disp_size = 64;
            op->eaflags |= EAF_WORDOFFS;
            break;
        default:
            nasm_nonfatal("invalid size specification in"
                          " effective address");
            break;
        }
    }
}