e1000_autoneg_timer(void *opaque)
{
    E1000State *s = opaque;
    s->nic->nc.link_down = false;
    e1000_link_up(s);
    s->phy_reg[PHY_STATUS] |= MII_SR_AUTONEG_COMPLETE;
    DBGOUT(PHY, "Auto negotiation is completed\n");
}