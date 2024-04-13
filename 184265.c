select_str_opcode(int mb_len, int str_len)
{
  int op;

  switch (mb_len) {
  case 1:
    switch (str_len) {
    case 1:  op = OP_STR_1; break;
    case 2:  op = OP_STR_2; break;
    case 3:  op = OP_STR_3; break;
    case 4:  op = OP_STR_4; break;
    case 5:  op = OP_STR_5; break;
    default: op = OP_STR_N; break;
    }
    break;

  case 2:
    switch (str_len) {
    case 1:  op = OP_STR_MB2N1; break;
    case 2:  op = OP_STR_MB2N2; break;
    case 3:  op = OP_STR_MB2N3; break;
    default: op = OP_STR_MB2N;  break;
    }
    break;

  case 3:
    op = OP_STR_MB3N;
    break;

  default:
    op = OP_STR_MBN;
    break;
  }

  return op;
}