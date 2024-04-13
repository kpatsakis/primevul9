float Utility::RoundToOneDecimal(float number)
{
    return QString::number(number, 'f', 1).toFloat();
}