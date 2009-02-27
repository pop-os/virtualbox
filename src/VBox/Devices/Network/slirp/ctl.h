#define CTL_CMD         0
#define CTL_EXEC        1
#define CTL_ALIAS       2
#define CTL_DNS         3
#define CTL_TFTP        CTL_DNS
#define CTL_BROADCAST   255

#define CTL_CHECK(x, ctl) (((x) & ~pData->netmask) == (ctl))
