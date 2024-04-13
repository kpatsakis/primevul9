static bool is_simm32(s64 value)
{
	return value == (s64) (s32) value;
}