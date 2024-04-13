inline int SubscriptToIndex(const NdArrayDesc<5>& desc, int indexes[5]) {
  return indexes[0] * desc.strides[0] + indexes[1] * desc.strides[1] +
         indexes[2] * desc.strides[2] + indexes[3] * desc.strides[3] +
         indexes[4] * desc.strides[4];
}