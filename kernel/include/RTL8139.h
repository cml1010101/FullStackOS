#ifndef RTL8139_H
#define RTL8139_H
#include <SmartOS.h>
#include <PCI.h>
#ifdef __cplusplus
#define RTL8139_CMD 0x37
#define DESCRIPTOR_EOR 0x40000000
#define DESCRIPTOR_OWN 0x80000000
#define NUM_RX 4
#define RX_BUFFER_SIZE 100
#define RTL8139_RXBUFFER_SIZE 8192 /* + 16 is allocated */

// RTL8139C registers
#define RTL_RBSTART 0x30 /* Receive Buffer Start Address (DWORD aligned) */
#define RTL_CR		0x37 /* command register */
#define RTL_CAPR	0x38 /* Current Address of Packet Read */
#define RTL_CBR		0x3a /* Current Buffer Address (Rx buffer) */
#define RTL_IMR		0x3c /* Interrupt Mask Register */
#define RTL_ISR		0x3e /* Interrupt Status Register */
#define RTL_TCR		0x40 /* Transmit Configuration Register */
#define RTL_RCR		0x44 /* Receive Configuration Register */
#define RTL_MPC		0x4c /* Missed Packet Counter */
#define RTL_CONFIG0 0x51 /* Configuration Register 0 */
#define RTL_CONFIG1 0x52 /* Configuration Register 1 */
#define RTL_CONFIG3 0x59 /* Configuration Register 3 */
#define RTL_CONFIG4 0x5a /* Configuration Register 4 */
#define RTL_BMCR	0x62 /* Basic Mode Control Register */
#define RTL_BMSR	0x64 /* Basic Mode Status Register */
#define RTL_CONFIG5 0xd8 /* Configuration Register 5 */

#define RTL_TSD0	0x10 /* Transmit Status of Descriptor 0 */
#define RTL_TSD1	0x14 /* Transmit Status of Descriptor 1 */
#define RTL_TSD2	0x18 /* Transmit Status of Descriptor 2 */
#define RTL_TSD3	0x1c /* Transmit Status of Descriptor 3 */
#define RTS_TSAD0	0x20 /* Transmit Start Address of Descriptor 0 (DWORD aligned) */
#define RTS_TSAD1	0x24 /* Transmit Start Address of Descriptor 1 (DWORD aligned) */
#define RTS_TSAD2	0x28 /* Transmit Start Address of Descriptor 2 (DWORD aligned) */
#define RTS_TSAD3	0x2c /* Transmit Start Address of Descriptor 3 (DWORD aligned) */
#define RTL_TSD_BASE  0x10 /* Used to calculate the register offset automatically */
#define RTL_TSAD_BASE 0x20 /* As above */

// Command register bits
#define RTL_RESET	(1 << 4) /* Software Reset */
#define RTL_RE		(1 << 3) /* Receiver Enable */
#define RTL_TE		(1 << 2) /* Transmitter Enable */
#define RTL_BUFE	(1 << 0) /* Receive Buffer Empty */

// IMR register bits
#define RTL_ROK (1 << 0)
#define RTL_RER (1 << 1)
#define RTL_TOK (1 << 2)
#define RTL_TER (1 << 3)

// RCR register bits
#define RTL_AAP  (1 << 0) /* Accept Physical Address Packets */
#define RTL_APM  (1 << 1) /* Accept Physical Match Packets */
#define RTL_AM   (1 << 2) /* Accept Multicast Packets */
#define RTL_AB	 (1 << 3)
#define RTL_TSD_TOK  (1 << 15)
#define RTL_TSD_OWN  (1 << 13)
#define RTL_TSD_BOTH ((1 << 15) | (1 << 13))
#define RTL_TSD_NONE 0
struct __attribute__((packed)) TXDesc
{
    uint8_t *buffer;
    uint32_t buffer_phys;
    uint16_t packet_length;
};
class RTL8139 : public EthernetDevice
{
private:
    void writeb(uint32_t reg, uint8_t val);
    uint8_t readb(uint32_t reg);
    void writes(uint32_t reg, uint16_t val);
    uint16_t reads(uint32_t reg);
    void writel(uint32_t reg, uint32_t val);
    uint32_t readl(uint32_t reg);
public:
    uint64_t mmioBase;
    uint16_t ioBase;
    uint8_t macAddress[6];
    TXDesc txDescs[4];
    uint64_t recieveBuffer, rxOffset, finishDescriptor, freeDescriptors, currentDescriptor;
    RTL8139(PCIDevice* dev);
    void sendPacket(Package* networkPackage);
    Package* recievePacket();
    uint8_t* getMAC();
    void handle();
};
extern RTL8139* defaultRTL8139;
#endif
#endif