static inline Quantum PerceptibleThreshold(const Quantum quantum,
  const double epsilon)
{
  double
    sign;

  sign=(double) quantum < 0.0 ? -1.0 : 1.0;
  if ((sign*quantum) >= epsilon)
    return(quantum);
  return((Quantum) (sign*epsilon));
}