
#ifndef __GS_USB_H_
#define __GS_USB_H_
#define GS_CAN_MODE_NORMAL        0
#define GS_CAN_MODE_LISTEN_ONLY   (1 << 0)
#define GS_CAN_MODE_LOOP_BACK     (1 << 1)
#define GS_CAN_MODE_TRIPLE_SAMPLE (1 << 2)
#define GS_CAN_MODE_ONE_SHOT      (1 << 3)
#define GS_CAN_MODE_HW_TIMESTAMP  (1 << 4)
/* #define GS_CAN_FEATURE_IDENTIFY              (1<<5) */
/* #define GS_CAN_FEATURE_USER_ID               (1<<6) */
#define GS_CAN_MODE_PAD_PKTS_TO_MAX_PKT_SIZE (1 << 7)
#define GS_CAN_MODE_FD                       (1 << 8) /* switch device to CAN-FD mode */
/* #define GS_CAN_FEATURE_REQ_USB_QUIRK_LPC546XX (1<<9) */
/* #define GS_CAN_FEATURE_BT_CONST_EXT          (1<<10) */

#define GS_CAN_FEATURE_LISTEN_ONLY              (1 << 0)
#define GS_CAN_FEATURE_LOOP_BACK                (1 << 1)
#define GS_CAN_FEATURE_TRIPLE_SAMPLE            (1 << 2)
#define GS_CAN_FEATURE_ONE_SHOT                 (1 << 3)
#define GS_CAN_FEATURE_HW_TIMESTAMP             (1 << 4)
#define GS_CAN_FEATURE_IDENTIFY                 (1 << 5)
#define GS_CAN_FEATURE_USER_ID                  (1 << 6)
#define GS_CAN_FEATURE_PAD_PKTS_TO_MAX_PKT_SIZE (1 << 7)
#define GS_CAN_FEATURE_FD                       (1 << 8) /* device supports CAN-FD */

/* request workaround for LPC546XX erratum USB.15:
 * let host driver add a padding byte to each USB frame
 */
#define GS_CAN_FEATURE_REQ_USB_QUIRK_LPC546XX (1 << 9)
/* device supports separate bit timing constants for CAN-FD
 * arbitration and data phase, see:
 * GS_USB_BREQ_BT_CONST_EXT and struct gs_device_bt_const_extended
 */
#define GS_CAN_FEATURE_BT_CONST_EXT (1 << 10)
#define GS_CAN_FEATURE_TERMINATION  (1 << 11)

#define GS_CAN_FLAG_OVERFLOW (1 << 0)
#define GS_CAN_FLAG_FD       (1 << 1) /* is a CAN-FD frame */
#define GS_CAN_FLAG_BRS      (1 << 2) /* bit rate switch (for CAN-FD frames) */
#define GS_CAN_FLAG_ESI      (1 << 3) /* error state indicator (for CAN-FD frames) */

#define CAN_EFF_FLAG 0x80000000U      /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U      /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U      /* error message frame */

#define CAN_ERR_DLC 8                 /* dlc for error message frames */

/* error class (mask) in can_id */
#define CAN_ERR_TX_TIMEOUT 0x00000001U /* TX timeout (by netdevice driver) */
#define CAN_ERR_LOSTARB    0x00000002U /* lost arbitration    / data[0]    */
#define CAN_ERR_CRTL       0x00000004U /* controller problems / data[1]    */
#define CAN_ERR_PROT       0x00000008U /* protocol violations / data[2..3] */
#define CAN_ERR_TRX        0x00000010U /* transceiver status  / data[4]    */
#define CAN_ERR_ACK        0x00000020U /* received no ACK on transmission */
#define CAN_ERR_BUSOFF     0x00000040U /* bus off */
#define CAN_ERR_BUSERROR   0x00000080U /* bus error (may flood!) */
#define CAN_ERR_RESTARTED  0x00000100U /* controller restarted */

/* arbitration lost in bit ... / data[0] */
#define CAN_ERR_LOSTARB_UNSPEC 0x00 /* unspecified */
                                    /* else bit number in bitstream */

/* error status of CAN-controller / data[1] */
#define CAN_ERR_CRTL_UNSPEC      0x00 /* unspecified */
#define CAN_ERR_CRTL_RX_OVERFLOW 0x01 /* RX buffer overflow */
#define CAN_ERR_CRTL_TX_OVERFLOW 0x02 /* TX buffer overflow */
#define CAN_ERR_CRTL_RX_WARNING  0x04 /* reached warning level for RX errors */
#define CAN_ERR_CRTL_TX_WARNING  0x08 /* reached warning level for TX errors */
#define CAN_ERR_CRTL_RX_PASSIVE  0x10 /* reached error passive status RX */
#define CAN_ERR_CRTL_TX_PASSIVE  0x20 /* reached error passive status TX */
                                      /* (at least one error counter exceeds */
                                      /* the protocol-defined level of 127)  */
#define CAN_ERR_CRTL_ACTIVE 0x40      /* recovered to error active state */

/* error in CAN protocol (type) / data[2] */
#define CAN_ERR_PROT_UNSPEC   0x00 /* unspecified */
#define CAN_ERR_PROT_BIT      0x01 /* single bit error */
#define CAN_ERR_PROT_FORM     0x02 /* frame format error */
#define CAN_ERR_PROT_STUFF    0x04 /* bit stuffing error */
#define CAN_ERR_PROT_BIT0     0x08 /* unable to send dominant bit */
#define CAN_ERR_PROT_BIT1     0x10 /* unable to send recessive bit */
#define CAN_ERR_PROT_OVERLOAD 0x20 /* bus overload */
#define CAN_ERR_PROT_ACTIVE   0x40 /* active error announcement */
#define CAN_ERR_PROT_TX       0x80 /* error occurred on transmission */

/* error in CAN protocol (location) / data[3] */
#define CAN_ERR_PROT_LOC_UNSPEC  0x00 /* unspecified */
#define CAN_ERR_PROT_LOC_SOF     0x03 /* start of frame */
#define CAN_ERR_PROT_LOC_ID28_21 0x02 /* ID bits 28 - 21 (SFF: 10 - 3) */
#define CAN_ERR_PROT_LOC_ID20_18 0x06 /* ID bits 20 - 18 (SFF: 2 - 0 )*/
#define CAN_ERR_PROT_LOC_SRTR    0x04 /* substitute RTR (SFF: RTR) */
#define CAN_ERR_PROT_LOC_IDE     0x05 /* identifier extension */
#define CAN_ERR_PROT_LOC_ID17_13 0x07 /* ID bits 17-13 */
#define CAN_ERR_PROT_LOC_ID12_05 0x0F /* ID bits 12-5 */
#define CAN_ERR_PROT_LOC_ID04_00 0x0E /* ID bits 4-0 */
#define CAN_ERR_PROT_LOC_RTR     0x0C /* RTR */
#define CAN_ERR_PROT_LOC_RES1    0x0D /* reserved bit 1 */
#define CAN_ERR_PROT_LOC_RES0    0x09 /* reserved bit 0 */
#define CAN_ERR_PROT_LOC_DLC     0x0B /* data length code */
#define CAN_ERR_PROT_LOC_DATA    0x0A /* data section */
#define CAN_ERR_PROT_LOC_CRC_SEQ 0x08 /* CRC sequence */
#define CAN_ERR_PROT_LOC_CRC_DEL 0x18 /* CRC delimiter */
#define CAN_ERR_PROT_LOC_ACK     0x19 /* ACK slot */
#define CAN_ERR_PROT_LOC_ACK_DEL 0x1B /* ACK delimiter */
#define CAN_ERR_PROT_LOC_EOF     0x1A /* end of frame */
#define CAN_ERR_PROT_LOC_INTERM  0x12 /* intermission */

/* error status of CAN-transceiver / data[4] */
/*                                             CANH CANL */
#define CAN_ERR_TRX_UNSPEC             0x00 /* 0000 0000 */
#define CAN_ERR_TRX_CANH_NO_WIRE       0x04 /* 0000 0100 */
#define CAN_ERR_TRX_CANH_SHORT_TO_BAT  0x05 /* 0000 0101 */
#define CAN_ERR_TRX_CANH_SHORT_TO_VCC  0x06 /* 0000 0110 */
#define CAN_ERR_TRX_CANH_SHORT_TO_GND  0x07 /* 0000 0111 */
#define CAN_ERR_TRX_CANL_NO_WIRE       0x40 /* 0100 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_BAT  0x50 /* 0101 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_VCC  0x60 /* 0110 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_GND  0x70 /* 0111 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_CANH 0x80 /* 1000 0000 */

enum gs_can_mode {
    /* reset a channel. turns it off */
    GS_CAN_MODE_RESET = 0,
    /* starts a channel */
    GS_CAN_MODE_START
};

enum gs_can_state {
    GS_CAN_STATE_ERROR_ACTIVE = 0,
    GS_CAN_STATE_ERROR_WARNING,
    GS_CAN_STATE_ERROR_PASSIVE,
    GS_CAN_STATE_BUS_OFF,
    GS_CAN_STATE_STOPPED,
    GS_CAN_STATE_SLEEPING
};
/* data types passed between host and device */
struct __packed gs_host_config {
    uint32_t byte_order;
};
/* All data exchanged between host and device is exchanged in host byte order,
 * thanks to the struct gs_host_config byte_order member, which is sent first
 * to indicate the desired byte order.
 */

typedef struct __packed __aligned(4) gs_device_config
{
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t icount;
    uint32_t sw_version;
    uint32_t hw_version;
}
gs_device_config_t;

struct __packed __aligned(4) gs_device_mode
{
    uint32_t mode;
    uint32_t flags;
};

struct __packed __aligned(4) gs_device_state
{
    uint32_t state;
    uint32_t rxerr;
    uint32_t txerr;
};

struct __packed __aligned(4) gs_device_bittiming
{
    uint32_t prop_seg;
    uint32_t phase_seg1;
    uint32_t phase_seg2;
    uint32_t sjw;
    uint32_t brp;
};

typedef struct __packed __aligned(4) gs_device_bt_const
{
    uint32_t feature;
    uint32_t fclk_can;
    uint32_t tseg1_min;
    uint32_t tseg1_max;
    uint32_t tseg2_min;
    uint32_t tseg2_max;
    uint32_t sjw_max;
    uint32_t brp_min;
    uint32_t brp_max;
    uint32_t brp_inc;
}
gs_device_bt_const_t;

struct __packed __aligned(4) gs_device_bt_const_extended
{
    uint32_t feature;
    uint32_t fclk_can;
    uint32_t tseg1_min;
    uint32_t tseg1_max;
    uint32_t tseg2_min;
    uint32_t tseg2_max;
    uint32_t sjw_max;
    uint32_t brp_min;
    uint32_t brp_max;
    uint32_t brp_inc;

    uint32_t dtseg1_min;
    uint32_t dtseg1_max;
    uint32_t dtseg2_min;
    uint32_t dtseg2_max;
    uint32_t dsjw_max;
    uint32_t dbrp_min;
    uint32_t dbrp_max;
    uint32_t dbrp_inc;
};

bool timestamps_enabled;
bool pad_pkts_to_max_pkt_size;

#endif