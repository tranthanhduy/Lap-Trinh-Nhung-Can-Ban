#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "NUC131.h"
#include "loopback.h"

#define PLL_CLOCK   50000000

//By DiTran_KhongTinhYeu

#define TEST_COUNT 16
uint8_t gDATABUF[2048];
wiz_NetInfo gWIZNETINFO;
wiz_NetInfo netInfo;


uint32_t g_au32SourceData[TEST_COUNT];
uint32_t g_au32DestinationData[TEST_COUNT];
volatile uint32_t g_u32TxDataCount;
volatile uint32_t g_u32RxDataCount;

void  wizchip_select(void);
void  wizchip_deselect(void);
void  wizchip_write(uint8_t wb);
uint8_t wizchip_read();
//int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);
//int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port);
// INIT NUC131
void SYS_Init(void);
void SPI_Init(void);

/* ------------- */
/* Main function */
/* ------------- */
int main()
{
	uint32_t u32DataCount;
	uint8_t tmp;
  uint8_t tmpstr[6] = {0,};
  int32_t ret = 0;
  uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
	uint8_t retVal, sockStatus;
  int16_t rcvLen;
  uint8_t rcvBuf[20], bufSize[] = {2, 2, 2, 2};
	
	/* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    /* Init SPI */
    SPI_Init();
	
		//..
//		reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
//		reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_select);  // CS must be tried with LOW.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #endif
#endif
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
		if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
       printf("WIZCHIP Initialized fail.\r\n");
       while(1);
    }
/*    for(i=0; i < _WIZCHIP_SOCK_NUM_ ; i++)
       if(getSn_TXBUF_SIZE(i) != 2 || getSn_RXBUF_SIZE(i) != 2)
       {
          printf("ERROR : WIZCHIP_MEM_SIZE\r\n");
          while(1);
       }*/

    do
    {
       if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
          printf("Unknown PHY Link stauts!");
    }while(tmp == PHY_LINK_OFF);
		
		//wizchip
		wizchip_init(bufSize, bufSize);
		wiz_NetInfo gWIZNETINFO = { 	.mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
                          .ip 	= {192, 168, 100, 192},					// IP address
                          .sn 	= {255, 255, 255, 0},					// Subnet mask
                          .gw 	= {192, 168, 100, 1}};					// Gateway address

//		netInfo.mac[0] = 0x00;
//		netInfo.mac[1] = 0x08;
//		netInfo.mac[2] = 0xdc;
//		netInfo.mac[3] = 0xab;
//		netInfo.mac[4] = 0xcd;
//		netInfo.mac[5] = 0xef;
//		
//		netInfo.ip[0] = 192;
//		netInfo.ip[1] = 168;
//		netInfo.ip[2] = 2;
//		netInfo.ip[3] = 192;
//		
//		netInfo.sn[0] = 255;
//		netInfo.sn[1] = 255;
//		netInfo.sn[2] = 255;
//		netInfo.sn[3] = 0;
//		
//		netInfo.gw[0] = 192;
//		netInfo.gw[1] = 168;
//		netInfo.gw[2] = 2;
//		netInfo.gw[3] = 1;
		
 //DISPLAY NETWORK
    ctlwizchip(CW_GET_ID,(void*)tmpstr);
    printf("=== %s NET CONF ===\r\n",(char*)tmpstr);
    printf("MAC:%02X.%02X.%02X.%02X.%02X.%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
          gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
    printf("GAR:%d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
    printf("SUB:%d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
    printf("SIP:%d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);

   while(1)
   {

      if( (ret = loopback_tcps(0,gDATABUF,3000)) < 0)
      {
         printf("SOCKET ERROR : %d\r\n", ret);
         if(ret != SOCKERR_SOCKCLOSED )
            while(1);
      }
      if( (ret = loopback_udps(1,gDATABUF,3000)) < 0)
      {
         printf("SOCKET ERROR : %d\r\n", ret);
         if(ret != SOCKERR_SOCKCLOSED )
            while(1);
      }
	};
}
		

/////////////////

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable external 12MHz XTAL */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Select HXT as the clock source of HCLK. HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));

    /* Select HXT as the clock source of UART0 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0_S_HCLK, MODULE_NoMsk);

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    /* Setup SPI0 multi-function pins */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC0_Msk | SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC2_Msk | SYS_GPC_MFP_PC3_Msk);
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC0_SPI0_SS0 | SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC2_SPI0_MISO0 | SYS_GPC_MFP_PC3_SPI0_MOSI0);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC0_Msk | SYS_ALT_MFP_PC1_Msk | SYS_ALT_MFP_PC2_Msk | SYS_ALT_MFP_PC3_Msk);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PC0_SPI0_SS0 | SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC2_SPI0_MISO0 | SYS_ALT_MFP_PC3_SPI0_MOSI0);
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure SPI0 as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 10MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_2, 16, 10000000);

    /* Enable the automatic hardware slave select function. Select the SPI0_SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
}


void  wizchip_select(void)
{
	// SPI_NSS = 0
   PC0 = 0;
}

void  wizchip_deselect(void)
{
	// SPI_NSS = 1
   PC0 = 1;
}

uint8_t wizchip_read()
{
	 uint8_t rb;
	 rb = SPI_READ_RX(SPI0);
   return rb;
}

void wizchip_write(uint8_t rb)
{
	/* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));
	SPI_WRITE_TX(SPI0, rb);
}
