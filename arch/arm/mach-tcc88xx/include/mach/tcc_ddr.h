/*
 * linux/arch/arm/mach-tccxxxx/include/mach/tcc_ddr.c
 *
 * Author:  <linux@telechips.com>
 * Created: November, 2010
 * Description: to change memory bus clock for Telechips chipset
 *
 * Copyright (C) Telechips, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __TCC_DDR__H__
#define __TCC_DDR__H__

#if defined(CONFIG_DRAM_LPDDR2)

/*===========================================================================

                           LPDDR2 Attributes Type

===========================================================================*/

/*---------------------------------------------------------------------------
 Clock Speed Type
---------------------------------------------------------------------------*/

#define LPDDR2_400	(400)
#define LPDDR2_533	(533)
#define LPDDR2_667	(667)
#define LPDDR2_800	(800)

/*---------------------------------------------------------------------------
 Page Size Type
---------------------------------------------------------------------------*/
enum 
{
	PAGE_1KB = 0,
	PAGE_2KB,
};

/*---------------------------------------------------------------------------
 Burst Length Type
---------------------------------------------------------------------------*/
enum
{
	BL_4 = 2,
	BL_8 = 3
};

/*---------------------------------------------------------------------------
 Read Burst Type
---------------------------------------------------------------------------*/
enum
{
	RBT_SEQUENTIAL = 0,
	RBT_INTERLEAVE,
};

/*---------------------------------------------------------------------------
 Memory Controller Output Driver Strength
---------------------------------------------------------------------------*/
enum
{
	MEMCTRL_OUTPUT_DRIVER_48_Ohm = 4,
	MEMCTRL_OUTPUT_DRIVER_40_Ohm = 5,
	MEMCTRL_OUTPUT_DRIVER_34_Ohm = 6,
	MEMCTRL_OUTPUT_DRIVER_30_Ohm = 7
};

/*---------------------------------------------------------------------------
 Memory Controller ODT(On-die-termination)
---------------------------------------------------------------------------*/
enum
{
	MEMCTRL_ODT_120_Ohm = 1,
	MEMCTRL_ODT_60_Ohm  = 2,
	MEMCTRL_ODT_40_Ohm  = 3,
	MEMCTRL_ODT_30_Ohm  = 4
};

/*---------------------------------------------------------------------------
 LPDDR2 Output Driver Strength
---------------------------------------------------------------------------*/
enum
{
	LPDDR2_DS_34_3_Ohm = 1,
	LPDDR2_DS_40_Ohm   = 2,
	LPDDR2_DS_48_Ohm   = 3,
	LPDDR2_DS_60_Ohm   = 4,
	LPDDR2_DS_80_Ohm   = 6,
	LPDDR2_DS_120_Ohm  = 7
};

/*---------------------------------------------------------------------------
 LPDDR2 Driving Strength
---------------------------------------------------------------------------*/
#define MEMCTRL_DDS       MEMCTRL_OUTPUT_DRIVER_30_Ohm
#define MEMCTRL_TERM      MEMCTRL_ODT_60_Ohm
#define LPDDR2_DS         LPDDR2_DS_40_Ohm

/*---------------------------------------------------------------------------
 LPDDR2 DLL low frequency mode
---------------------------------------------------------------------------*/
#define LPDDR2_DLL_LOW_FREQ_MODE_LIMIT     140


#if defined(CONFIG_LPDDR2_K4P4G324EB_AGC1) /*==============================*/

/*---------------------------------------------------------------------------
 LPDDR2 Configuation
---------------------------------------------------------------------------*/
#define LPDDR2_PHYSICAL_CHIP_NUM           1
#define LPDDR2_LOGICAL_CHIP_NUM            1
#define LPDDR2_MAX_SPEED            LPDDR2_800
#define LPDDR2_BURST_LEN                BL_4
#define LPDDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define LPDDR2_EA_MB_SIZE                512
#define LPDDR2_TOTAL_MB_SIZE             (LPDDR2_EA_MB_SIZE*LPDDR2_PHYSICAL_CHIP_NUM)
#define LPDDR2_ROWBITS                    14
#define LPDDR2_COLBITS                    10
#define LPDDR2_BANK_BITS                   3
#define LPDDR2_BANK_NUM                    8
#define LPDDR2_PHYSICAL_DATA_BITS         32
#define LPDDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 LPDDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define LPDDR2_tRFC_ps                130000
#define LPDDR2_tREFI_ps              3900000
#define LPDDR2_tRRD_ps                 10000
#define LPDDR2_tRRD_ck                     2
#define LPDDR2_tRAS_ps                 42000
#define LPDDR2_tRAS_ck                     3
#define LPDDR2_tRP_ps                  21000
#define LPDDR2_tRP_ck                      3
#define LPDDR2_tRC_ps                  (LPDDR2_tRAS_ps+LPDDR2_tRP_ps)
#define LPDDR2_tRC_ck                  (LPDDR2_tRAS_ck+LPDDR2_tRP_ck)
#define LPDDR2_tRCD_ps                 18000
#define LPDDR2_tRCD_ck                     3
#define LPDDR2_tWTR_ps                  7500
#define LPDDR2_tWTR_ck                     2
#define LPDDR2_tWR_ps                  15000
#define LPDDR2_tWR_ck                      3
#define LPDDR2_tRTP_ps                  7500
#define LPDDR2_tRTP_ck                     2
#define LPDDR2_tFAW_ps                 50000
#define LPDDR2_tFAW_ck                     8
#define LPDDR2_tXSR_ps                 (LPDDR2_tRFC_ps+10000)
#define LPDDR2_tXSR_ck                     2
#define LPDDR2_tXP_ps                   7500
#define LPDDR2_tXP_ck                      2
#define LPDDR2_tCKE_ps                  7500
#define LPDDR2_tCKE_ck                     3
#define LPDDR2_tMRD_ck                     5
#define LPDDR2_tRL_ck                      6
#define LPDDR2_tWL_ck                      3

#else
/*-------------------------------------------------------------------------*/
	#error Not Selectied LPDDR2 Memory Type
#endif

/*---------------------------------------------------------------------------
 PHY ZQ control value
---------------------------------------------------------------------------*/
#define PHYZQCTRL	( (0xE38<<20) \
                    | (0x2<<17)   \
                    | (0x5<<14)   \
                    | (MEMCTRL_TERM<<11)   \
                    | (MEMCTRL_DDS<<8)    \
                    | (0x0<<4)    \
                    | (0x0<<2)    \
                    | (0x0<<1)    \
                    | (0x0<<0) )



#elif defined(CONFIG_DRAM_DDR2)

/*===========================================================================

                           DDR2 Attributes Type

===========================================================================*/

/*---------------------------------------------------------------------------
 Clock Speed Type
---------------------------------------------------------------------------*/

#define DDR2_400	(400)
#define DDR2_533	(533)
#define DDR2_667	(667)
#define DDR2_800	(800)

/*---------------------------------------------------------------------------
 Page Size Type
---------------------------------------------------------------------------*/
enum 
{
	PAGE_1KB = 0,
	PAGE_2KB,
};

/*---------------------------------------------------------------------------
 Burst Length Type
---------------------------------------------------------------------------*/
enum
{
	BL_4 = 2,
	BL_8 = 3
};

/*---------------------------------------------------------------------------
 Read Burst Type
---------------------------------------------------------------------------*/
enum
{
	RBT_SEQUENTIAL = 0,
	RBT_INTERLEAVE,
};

/*---------------------------------------------------------------------------
 Memory Controller Output Driver Strength
---------------------------------------------------------------------------*/
enum
{
	MEMCTRL_OUTPUT_DRIVER_48_Ohm = 4,
	MEMCTRL_OUTPUT_DRIVER_40_Ohm = 5,
	MEMCTRL_OUTPUT_DRIVER_34_Ohm = 6,
	MEMCTRL_OUTPUT_DRIVER_30_Ohm = 7
};

/*---------------------------------------------------------------------------
 Memory Controller ODT(On-die-termination)
---------------------------------------------------------------------------*/
enum
{
	MEMCTRL_ODT_120_Ohm = 1,
	MEMCTRL_ODT_60_Ohm  = 2,
	MEMCTRL_ODT_40_Ohm  = 3,
	MEMCTRL_ODT_30_Ohm  = 4
};

/*---------------------------------------------------------------------------
 DDR2 Output Driver Strength
---------------------------------------------------------------------------*/
enum
{
	DDR2_DIC_FULL = 0,
	DDR2_DIC_HALF = 1
};

/*---------------------------------------------------------------------------
 DDR2 ODT(On-die-termination)
---------------------------------------------------------------------------*/
enum
{
	DDR2_RTT_DISABLE = 0,
	DDR2_RTT_75_Ohm  = 1,
	DDR2_RTT_150_Ohm = 0x10,
	DDR2_RTT_50_Ohm  = 0x11
};

/*---------------------------------------------------------------------------
 DDR2 Driving Strength
---------------------------------------------------------------------------*/
#define MEMCTRL_DDS     MEMCTRL_OUTPUT_DRIVER_30_Ohm
#define MEMCTRL_TERM    MEMCTRL_ODT_60_Ohm
#define DDR2_DIC        DDR2_DIC_HALF
#define DDR2_ODT        DDR2_RTT_75_Ohm

/*---------------------------------------------------------------------------
 DDR2 ODT enable minimum frequency
---------------------------------------------------------------------------*/
#define DDR2_ODT_ENABLE_MIN_FREQ       200


#if defined(CONFIG_DDR2_HXB18T2G160AF) /*==================================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#define DDR2_PHYSICAL_CHIP_NUM           2
#define DDR2_LOGICAL_CHIP_NUM            1
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          5
#define DDR2_PAGE_SIZE            PAGE_2KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                256
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    14
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                175000
#define DDR2_tREFI_ps              7800000
#define DDR2_tRRD_ps                 10000
#define DDR2_tRRD_ck                     1
#define DDR2_tRAS_ps                 45000
#define DDR2_tRAS_ck                     1
#define DDR2_tRC_ps                  57500
#define DDR2_tRC_ck                      1
#define DDR2_tRCD_ps                 12500
#define DDR2_tRCD_ck                     1
#define DDR2_tRP_ps                  12500
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     2
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#define DDR2_tFAW_ps                 45000
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#elif defined(CONFIG_DDR2_HY5PS1G1631CFPS6) /*==============================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#if defined(CONFIG_TCC_MEM_256MB)
#define DDR2_PHYSICAL_CHIP_NUM           2
#define DDR2_LOGICAL_CHIP_NUM            1
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          6
#define DDR2_PAGE_SIZE            PAGE_2KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                128
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    13
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32
#elif defined(CONFIG_TCC_MEM_512MB)
#define DDR2_PHYSICAL_CHIP_NUM           4
#define DDR2_LOGICAL_CHIP_NUM            2
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          6
#define DDR2_PAGE_SIZE            PAGE_2KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                128
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    13
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32
#else
#error "not supported size"
#endif

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                127500
#define DDR2_tREFI_ps              7800000
#define DDR2_tRRD_ps                 10000
#define DDR2_tRRD_ck                     1
#define DDR2_tRAS_ps                 45000
#define DDR2_tRAS_ck                     1
#define DDR2_tRC_ps                  60000
#define DDR2_tRC_ck                      1
#define DDR2_tRCD_ps                 15000
#define DDR2_tRCD_ck                     1
#define DDR2_tRP_ps                  15000
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     2
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#define DDR2_tFAW_ps                 45000
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#elif defined(CONFIG_DDR2_HY5PS1G831CFPS6) /*==============================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#define DDR2_PHYSICAL_CHIP_NUM           4
#define DDR2_LOGICAL_CHIP_NUM            1
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          6
#define DDR2_PAGE_SIZE            PAGE_1KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                128
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    14
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                127500
#define DDR2_tREFI_ps              7800000
#define DDR2_tRRD_ps                  7500
#define DDR2_tRRD_ck                     1
#define DDR2_tRAS_ps                 45000
#define DDR2_tRAS_ck                     1
#define DDR2_tRC_ps                  60000
#define DDR2_tRC_ck                      1
#define DDR2_tRCD_ps                 15000
#define DDR2_tRCD_ck                     1
#define DDR2_tRP_ps                  15000
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     2
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#define DDR2_tFAW_ps                 35000
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#elif defined(CONFIG_DDR2_K4T1G164QE_HCF7) /*==============================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#define DDR2_PHYSICAL_CHIP_NUM           4
#define DDR2_LOGICAL_CHIP_NUM            2
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          6
#define DDR2_PAGE_SIZE            PAGE_2KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                128
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    13
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                127500
#define DDR2_tREFI_ps              7800000
#define DDR2_tRRD_ps                 10000
#define DDR2_tRRD_ck                     1
#define DDR2_tRAS_ps                 45000
#define DDR2_tRAS_ck                     1
#define DDR2_tRC_ps                  60000
#define DDR2_tRC_ck                      1
#define DDR2_tRCD_ps                 15000
#define DDR2_tRCD_ck                     1
#define DDR2_tRP_ps                  15000
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     2
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#define DDR2_tFAW_ps                 45000
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#elif defined(CONFIG_DDR2_K4T1G084QF_BCE7) /*==============================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#define DDR2_PHYSICAL_CHIP_NUM           4
#define DDR2_LOGICAL_CHIP_NUM            1
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          5
#define DDR2_PAGE_SIZE            PAGE_1KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                128
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    14
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                127500
#define DDR2_tREFI_ps              7800000
#define DDR2_tRRD_ps                  7500
#define DDR2_tRRD_ck                     1
#define DDR2_tRAS_ps                 45000
#define DDR2_tRAS_ck                     1
#define DDR2_tRC_ps                  57500
#define DDR2_tRC_ck                      1
#define DDR2_tRCD_ps                 12500
#define DDR2_tRCD_ck                     1
#define DDR2_tRP_ps                  12500
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     3
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#define DDR2_tFAW_ps                 35000
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#elif defined(CONFIG_DDR2_CT83486C1) /*====================================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#define DDR2_PHYSICAL_CHIP_NUM           2
#define DDR2_LOGICAL_CHIP_NUM            1
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          6
#define DDR2_PAGE_SIZE            PAGE_2KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                256
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    14
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS         16
#define DDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                127500
#define DDR2_tREFI_ps              7800000

#if (DDR2_CL == 6)
#define DDR2_tRRD_ps                 10000
#else
#define DDR2_tRRD_ps                  7500
#endif
#define DDR2_tRRD_ck                     1
#if (DDR2_CL == 6)
#define DDR2_tRAS_ps                 40000
#else
#define DDR2_tRAS_ps                 45000
#endif
#define DDR2_tRAS_ck                     1
#if (DDR2_CL == 6)
#define DDR2_tRC_ps                  55000
#else
#define DDR2_tRC_ps                  52500
#endif
#define DDR2_tRC_ck                      1
#if (DDR2_CL == 6)
#define DDR2_tRCD_ps                 15000
#else
#define DDR2_tRCD_ps                 12500
#endif
#define DDR2_tRCD_ck                     1
#if (DDR2_CL == 6)
#define DDR2_tRP_ps                  15000
#else
#define DDR2_tRP_ps                  12500
#endif
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     2
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#if (DDR2_CL == 6)
#define DDR2_tFAW_ps                 45000
#else
#define DDR2_tFAW_ps                 35000
#endif
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#elif defined(CONFIG_DDR2_MT47H256M8EB25E) /*==============================*/

/*---------------------------------------------------------------------------
 DDR2 Configuation
---------------------------------------------------------------------------*/
#define DDR2_PHYSICAL_CHIP_NUM           4
#define DDR2_LOGICAL_CHIP_NUM            1
#define DDR2_MAX_SPEED            DDR2_800
#define DDR2_CL                          5
#define DDR2_PAGE_SIZE            PAGE_1KB
#define DDR2_BURST_LEN                BL_4
#define DDR2_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR2_EA_MB_SIZE                256
#define DDR2_TOTAL_MB_SIZE             (DDR2_EA_MB_SIZE*DDR2_PHYSICAL_CHIP_NUM)
#define DDR2_ROWBITS                    15
#define DDR2_COLBITS                    10
#define DDR2_BANK_BITS                   3
#define DDR2_BANK_NUM                    8
#define DDR2_PHYSICAL_DATA_BITS          8
#define DDR2_LOGICAL_DATA_BITS          32

/*---------------------------------------------------------------------------
 DDR2 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR2_tRFC_ps                195000
#define DDR2_tREFI_ps              7800000
#define DDR2_tRRD_ps                  7500
#define DDR2_tRRD_ck                     1
#define DDR2_tRAS_ps                 45000
#define DDR2_tRAS_ck                     1
#define DDR2_tRC_ps                  57500
#define DDR2_tRC_ck                      1
#define DDR2_tRCD_ps                 12500
#define DDR2_tRCD_ck                     1
#define DDR2_tRP_ps                  12500
#define DDR2_tRP_ck                      1
#define DDR2_tWTR_ps                  7500
#define DDR2_tWTR_ck                     2
#define DDR2_tWR_ps                  15000
#define DDR2_tWR_ck                      1
#define DDR2_tRTP_ps                  7500
#define DDR2_tRTP_ck                     2
#define DDR2_tFAW_ps                 35000
#define DDR2_tFAW_ck                     1
#define DDR2_tXSR_ck                   200
#define DDR2_tXP_ck                      2
#define DDR2_tCKE_ck                     3
#define DDR2_tMRD_ck                     2

#else
/*-------------------------------------------------------------------------*/
	#error Not Selectied DDR2 Memory Type
#endif

/*---------------------------------------------------------------------------
 PHY ZQ control value
---------------------------------------------------------------------------*/
#define PHYZQCTRL	( (0xE38<<20) \
                    | (0x2<<17)   \
                    | (0x5<<14)   \
                    | (MEMCTRL_TERM<<11)   \
                    | (MEMCTRL_DDS<<8)    \
                    | (0x3<<4)    \
                    | (0x0<<2)    \
                    | (0x0<<1)    \
                    | (0x0<<0) )



#elif defined(CONFIG_DRAM_DDR3)

/*===========================================================================

                           DDR3 Attributes Type

===========================================================================*/

//Bruce, 101102, Clock 변경시, WR과 CL의 변경사항이 없어도 항상 MRS Setting을 다시 한다.
#define MRS_ALWAYS_SETTING

/*---------------------------------------------------------------------------
 Clock Speed Type
---------------------------------------------------------------------------*/

#define DDR3_800	(800)
#define DDR3_1066	(1066)
#define DDR3_1333	(1333)
#define DDR3_1600	(1600)
#define DDR3_1866	(1866)
#define DDR3_2133	(2133)

/*---------------------------------------------------------------------------
 Additive Latency Type
---------------------------------------------------------------------------*/
enum
{
	AL_DISABLED = 0,
	AL_CL_MINUS_ONE,
	AL_CL_MINUS_TWO
};

/*---------------------------------------------------------------------------
 Size Type
---------------------------------------------------------------------------*/
enum
{
	SIZE_512MBIT = 0,
	SIZE_1GBIT,
	SIZE_2GBIT,
	SIZE_4GBIT,
	SIZE_8GBIT,
};

/*---------------------------------------------------------------------------
 Page Size Type
---------------------------------------------------------------------------*/
enum 
{
	PAGE_1KB = 0,
	PAGE_2KB,
};

/*---------------------------------------------------------------------------
 Burst Length Type
---------------------------------------------------------------------------*/
enum
{
	BL_8 = 0,
	BL_BC4_OR_8,//Not supported by DDR2/DDR3 Controller
	BL_BC4,
};

/*---------------------------------------------------------------------------
 Read Burst Type
---------------------------------------------------------------------------*/
enum
{
	RBT_SEQUENTIAL = 0,
	RBT_INTERLEAVE,
};

/*---------------------------------------------------------------------------
 Write recovery for autoprecharge in MR0
---------------------------------------------------------------------------*/
enum{
	WR_5 = 1,
	WR_6,
	WR_7,
	WR_8,
	WR_10,
	WR_12
};

/*---------------------------------------------------------------------------
 DLL Control for Precharge PD
---------------------------------------------------------------------------*/
enum
{
	SLOW_EXIT = 0,
	FAST_EXIT
};

/*---------------------------------------------------------------------------
 Memory Controller Output Driver Strength
---------------------------------------------------------------------------*/
enum
{
	MEMCTRL_OUTPUT_DRIVER_48_Ohm = 4,
	MEMCTRL_OUTPUT_DRIVER_40_Ohm = 5,
	MEMCTRL_OUTPUT_DRIVER_34_Ohm = 6,
	MEMCTRL_OUTPUT_DRIVER_30_Ohm = 7
};

/*---------------------------------------------------------------------------
 Memory Controller ODT(On-die-termination)
---------------------------------------------------------------------------*/
enum
{
	MEMCTRL_ODT_120_Ohm = 1,
	MEMCTRL_ODT_60_Ohm  = 2,
	MEMCTRL_ODT_40_Ohm  = 3,
	MEMCTRL_ODT_30_Ohm  = 4
};

/*---------------------------------------------------------------------------
 DDR3 Output Driver Strength , RZQ=240 Ohm
---------------------------------------------------------------------------*/
enum
{
	DDR3_DIC_RZQ_DIV_6 = 0, //40 Ohm
	DDR3_DIC_RZQ_DIV_7 = 1  //34 Ohm
};

/*---------------------------------------------------------------------------
 DDR3 ODT(On-die-termination) , RZQ=240 Ohm
---------------------------------------------------------------------------*/
enum
{
	DDR3_RTT_DISABLE    = 0,
	DDR3_RTT_RZQ_DIV_4  = 1,    //60 Ohm
	DDR3_RTT_RZQ_DIV_2  = 0x10, //120 Ohm
	DDR3_RTT_RZQ_DIV_6  = 0x11, //40 Ohm
	DDR3_RTT_RZQ_DIV_12 = 0x80, // 20 Ohm
	DDR3_RTT_RZQ_DIV_8  = 0x81  // 30 Ohm
};

/*---------------------------------------------------------------------------
 DDR3 Driving Strength
---------------------------------------------------------------------------*/
#define MEMCTRL_DDS     MEMCTRL_OUTPUT_DRIVER_34_Ohm
#define MEMCTRL_TERM    MEMCTRL_ODT_60_Ohm
#define DDR3_DIC        DDR3_DIC_RZQ_DIV_7
#define DDR3_ODT        DDR3_RTT_RZQ_DIV_4

#if defined(CONFIG_DDR3_K4B2G1646C_HCK0) /*================================*/

/*---------------------------------------------------------------------------
 DDR3 Configuation
---------------------------------------------------------------------------*/
#define DDR3_PHYSICAL_CHIP_NUM           2
#define DDR3_LOGICAL_CHIP_NUM            1
#define DDR3_MAX_SPEED           DDR3_1600
#define DDR3_CL                         11
#define DDR3_AL                AL_DISABLED
#define DDR3_PAGE_SIZE            PAGE_2KB
#define DDR3_BURST_LEN                BL_8
#define DDR3_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR3_EA_BIT_SIZE        SIZE_2GBIT
#define DDR3_TOTAL_MB_SIZE             512
#define DDR3_ROWBITS                    14
#define DDR3_COLBITS                    10
#define DDR3_BANK_NUM                    8

/*---------------------------------------------------------------------------
 DDR3 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR3_tRFC_ps                160000
#define DDR3_tREFI_ps              7800000
#define DDR3_tRCD_ps                 13750
#define DDR3_tRCD_ck                     1
#define DDR3_tRP_ps                  13750
#define DDR3_tRP_ck                      1
#define DDR3_tRC_ps                  48750
#define DDR3_tRC_ck                      1
#define DDR3_tRAS_ps                 35000
#define DDR3_tRAS_ck                     1
#define DDR3_tRTP_ps                  7500
#define DDR3_tRTP_ck                     4
#define DDR3_tWTR_ps                  7500
#define DDR3_tWTR_ck                     4
#define DDR3_tWR_ps                  15000
#define DDR3_tWR_ck                      1
#define DDR3_tRRD_ps                  7500
#define DDR3_tRRD_ck                     4
#define DDR3_tFAW_ps                 40000
#define DDR3_tFAW_ck                     1
#define DDR3_tXS_ps    (DDR3_tRFC_ps+10000)
#define DDR3_tXS_ck                      5
#define DDR3_tXP_ps                   6000
#define DDR3_tXP_ck                      3
#define DDR3_tCKE_ps                  5000
#define DDR3_tCKE_ck                     3
#define DDR3_tMRD_ck                     4

#elif defined(CONFIG_DDR3_K4B1G1646E_HCH9) /*==============================*/

/*---------------------------------------------------------------------------
 DDR3 Configuation
---------------------------------------------------------------------------*/
#define DDR3_PHYSICAL_CHIP_NUM           2
#define DDR3_LOGICAL_CHIP_NUM            1
#define DDR3_MAX_SPEED           DDR3_1333
#define DDR3_CL                          9
#define DDR3_AL                AL_DISABLED
#define DDR3_PAGE_SIZE            PAGE_2KB
#define DDR3_BURST_LEN                BL_8
#define DDR3_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR3_EA_BIT_SIZE        SIZE_1GBIT
#define DDR3_TOTAL_MB_SIZE             256
#define DDR3_ROWBITS                    13
#define DDR3_COLBITS                    10
#define DDR3_BANK_NUM                    8

/*---------------------------------------------------------------------------
 DDR3 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR3_tRFC_ps                110000
#define DDR3_tREFI_ps              7800000
#define DDR3_tRCD_ps                 13500
#define DDR3_tRCD_ck                     1
#define DDR3_tRP_ps                  13500
#define DDR3_tRP_ck                      1
#define DDR3_tRC_ps                  49500
#define DDR3_tRC_ck                      1
#define DDR3_tRAS_ps                 36000
#define DDR3_tRAS_ck                     1
#define DDR3_tRTP_ps                  7500
#define DDR3_tRTP_ck                     4
#define DDR3_tWTR_ps                  7500
#define DDR3_tWTR_ck                     4
#define DDR3_tWR_ps                  15000
#define DDR3_tWR_ck                      1
#define DDR3_tRRD_ps                  7500
#define DDR3_tRRD_ck                     4
#define DDR3_tFAW_ps                 45000
#define DDR3_tFAW_ck                     1
#define DDR3_tXS_ps    (DDR3_tRFC_ps+10000)
#define DDR3_tXS_ck                      5
#define DDR3_tXP_ps                   6000
#define DDR3_tXP_ck                      3
#define DDR3_tCKE_ps                  5625
#define DDR3_tCKE_ck                     3
#define DDR3_tMRD_ck                     4

#elif defined(CONFIG_DDR3_H5TQ2G63BFR_H9C) /*================================*/

/*---------------------------------------------------------------------------
 DDR3 Configuation
---------------------------------------------------------------------------*/
#define DDR3_PHYSICAL_CHIP_NUM           2
#define DDR3_LOGICAL_CHIP_NUM            1
#define DDR3_MAX_SPEED           DDR3_1333
#define DDR3_CL                          9
#define DDR3_AL                AL_DISABLED
#define DDR3_PAGE_SIZE            PAGE_2KB
#define DDR3_BURST_LEN                BL_8
#define DDR3_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR3_EA_BIT_SIZE        SIZE_2GBIT
#define DDR3_TOTAL_MB_SIZE             512
#define DDR3_ROWBITS                    14
#define DDR3_COLBITS                    10
#define DDR3_BANK_NUM                    8

/*---------------------------------------------------------------------------
 DDR3 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR3_tRFC_ps                160000
#define DDR3_tREFI_ps              7800000
#define DDR3_tRCD_ps                 13500
#define DDR3_tRCD_ck                     1
#define DDR3_tRP_ps                  13500
#define DDR3_tRP_ck                      1
#define DDR3_tRC_ps                  49500
#define DDR3_tRC_ck                      1
#define DDR3_tRAS_ps                 36000
#define DDR3_tRAS_ck                     1
#define DDR3_tRTP_ps                  7500
#define DDR3_tRTP_ck                     4
#define DDR3_tWTR_ps                  7500
#define DDR3_tWTR_ck                     4
#define DDR3_tWR_ps                  15000
#define DDR3_tWR_ck                      1
#define DDR3_tRRD_ps                  7500
#define DDR3_tRRD_ck                     5
#define DDR3_tFAW_ps                 45000
#define DDR3_tFAW_ck                    30
#define DDR3_tXS_ps    (DDR3_tRFC_ps+10000)
#define DDR3_tXS_ck                      5
#define DDR3_tXP_ps                   6000
#define DDR3_tXP_ck                      3
#define DDR3_tCKE_ps                  5625
#define DDR3_tCKE_ck                     3
#define DDR3_tMRD_ck                     4

#elif defined(CONFIG_DDR3_H5TQ2G83BFR_H9C) /*================================*/

/*---------------------------------------------------------------------------
 DDR3 Configuation
---------------------------------------------------------------------------*/
#define DDR3_PHYSICAL_CHIP_NUM           4
#define DDR3_LOGICAL_CHIP_NUM            1
#define DDR3_MAX_SPEED           DDR3_1333
#define DDR3_CL                          9
#define DDR3_AL                AL_DISABLED
#define DDR3_PAGE_SIZE            PAGE_1KB
#define DDR3_BURST_LEN                BL_8
#define DDR3_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR3_EA_BIT_SIZE        SIZE_2GBIT
#define DDR3_TOTAL_MB_SIZE            1024
#define DDR3_ROWBITS                    15
#define DDR3_COLBITS                    10
#define DDR3_BANK_NUM                    8

/*---------------------------------------------------------------------------
 DDR3 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR3_tRFC_ps                160000
#define DDR3_tREFI_ps              7800000
#define DDR3_tRCD_ps                 13500
#define DDR3_tRCD_ck                     1
#define DDR3_tRP_ps                  13500
#define DDR3_tRP_ck                      1
#define DDR3_tRC_ps                  49500
#define DDR3_tRC_ck                      1
#define DDR3_tRAS_ps                 36000
#define DDR3_tRAS_ck                     1
#define DDR3_tRTP_ps                  7500
#define DDR3_tRTP_ck                     4
#define DDR3_tWTR_ps                  7500
#define DDR3_tWTR_ck                     4
#define DDR3_tWR_ps                  15000
#define DDR3_tWR_ck                      1
#define DDR3_tRRD_ps                  7500
#define DDR3_tRRD_ck                     4
#define DDR3_tFAW_ps                 45000
#define DDR3_tFAW_ck                    20
#define DDR3_tXS_ps    (DDR3_tRFC_ps+10000)
#define DDR3_tXS_ck                      5
#define DDR3_tXP_ps                   6000
#define DDR3_tXP_ck                      3
#define DDR3_tCKE_ps                  5625
#define DDR3_tCKE_ck                     3
#define DDR3_tMRD_ck                     4

#elif defined(CONFIG_DDR3_H5TQ1G83BFR_H9C) /*================================*/

/*---------------------------------------------------------------------------
 DDR3 Configuation
---------------------------------------------------------------------------*/
#define DDR3_PHYSICAL_CHIP_NUM           4
#define DDR3_LOGICAL_CHIP_NUM            1
#define DDR3_MAX_SPEED           DDR3_1333
#define DDR3_CL                          9
#define DDR3_AL                AL_DISABLED
#define DDR3_PAGE_SIZE            PAGE_1KB
#define DDR3_BURST_LEN                BL_8
#define DDR3_READ_BURST_TYPE RBT_SEQUENTIAL
#define DDR3_EA_BIT_SIZE        SIZE_1GBIT
#define DDR3_TOTAL_MB_SIZE             512
#define DDR3_ROWBITS                    14
#define DDR3_COLBITS                    10
#define DDR3_BANK_NUM                    8

/*---------------------------------------------------------------------------
 DDR3 Access Timing Parameters
---------------------------------------------------------------------------*/
#define DDR3_tRFC_ps                110000
#define DDR3_tREFI_ps              7800000
#define DDR3_tRCD_ps                 13500
#define DDR3_tRCD_ck                     1
#define DDR3_tRP_ps                  13500
#define DDR3_tRP_ck                      1
#define DDR3_tRC_ps                  49500
#define DDR3_tRC_ck                      1
#define DDR3_tRAS_ps                 36000
#define DDR3_tRAS_ck                    24
#define DDR3_tRTP_ps                  7500
#define DDR3_tRTP_ck                     4
#define DDR3_tWTR_ps                  7500
#define DDR3_tWTR_ck                     4
#define DDR3_tWR_ps                  15000
#define DDR3_tWR_ck                      1
#define DDR3_tRRD_ps                  6000
#define DDR3_tRRD_ck                     4
#define DDR3_tFAW_ps                 30000
#define DDR3_tFAW_ck                    20
#define DDR3_tXS_ps    (DDR3_tRFC_ps+10000)
#define DDR3_tXS_ck                      5
#define DDR3_tXP_ps                   6000
#define DDR3_tXP_ck                      3
#define DDR3_tCKE_ps                  5625
#define DDR3_tCKE_ck                     3
#define DDR3_tMRD_ck                     4

#else
/*-------------------------------------------------------------------------*/
#error "not selected"
#endif

#endif /* CONFIG_DRAM_DDR3 */

#endif	/* __TCC_DDR__H__ */
/************* end of file *************************************************************/
