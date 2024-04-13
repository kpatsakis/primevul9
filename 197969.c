static int _getClosest2Power(double value)
{
    return (int)(log(value) / log(2.0) + 0.5);
}