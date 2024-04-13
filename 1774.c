inline void NDOpsHelper(const NdArrayDesc<N>& output, const Calc& calc) {
  int indexes[N] = {0};
  NDOpsHelperImpl<N, 0, Calc>(output, calc, indexes);
}