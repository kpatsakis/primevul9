static int q2mbl(int x) {
  if (x < 20) x = 20;

  x = 50 + (x - 50) * 10 / 8;
  return x * x / 3;
}