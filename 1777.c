inline int SubscriptToIndex(const NdArrayDesc<8>& desc, int indexes[8]) {
  return indexes[0] * desc.strides[0] + indexes[1] * desc.strides[1] +
         indexes[2] * desc.strides[2] + indexes[3] * desc.strides[3] +
         indexes[4] * desc.strides[4] + indexes[5] * desc.strides[5] +
         indexes[6] * desc.strides[6] + indexes[7] * desc.strides[7];
}