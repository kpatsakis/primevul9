typename std::enable_if<DIM != N - 1, void>::type NDOpsHelperImpl(
    const NdArrayDesc<N>& output, const Calc& calc, int indexes[N]) {
  for (indexes[DIM] = 0; indexes[DIM] < output.extents[DIM]; ++indexes[DIM]) {
    NDOpsHelperImpl<N, DIM + 1, Calc>(output, calc, indexes);
  }
}