AP_DECLARE(int) ap_exists_scoreboard_image(void)
{
    return (ap_scoreboard_image ? 1 : 0);
}