/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_mod_hal_serdes_avg_complex_regs.h
 *
 * @brief srd_complex registers
 *
 * This file was auto-generated by RegGen v1.2.1
 *
 */

#ifndef __AL_HAL_SERDES_AVG_COMPLEX_REGS_H__
#define __AL_HAL_SERDES_AVG_COMPLEX_REGS_H__

#include "al_mod_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

/*
 * General configuration and status
 */
struct al_mod_srd_complex_gen {
	/* [0x0] SERDES registers Version */
	uint32_t version;
	/* [0x4] */
	uint32_t rsrvd_0[63];
};

/*
 * Taranaki specific
 */
struct al_mod_srd_complex_tnk_gen {
	/* [0x0] SERDES MUX */
	uint32_t mux;
	/* [0x4] select tx clk */
	uint32_t tx_clk;
	/* [0x8] */
	uint32_t rsrvd_0[6];
};

/*
 * tpr_specific
 */
struct al_mod_srd_complex_tpr_cfg {
	/* [0x0] SERDES MUX */
	uint32_t main_sel;
	/* [0x4] select tx clk */
	uint32_t main_mux;
	/* [0x8] SERDES MUX */
	uint32_t multi_sel;
	/* [0xc] select tx clk */
	uint32_t reserv_mux;
	/* [0x10] SERDES MUX */
	uint32_t hs_sel;
	/* [0x14] Lane select. The user must make sure each lane receive a different SerDes */
	uint32_t hs_mux;
	/* [0x18] */
	uint32_t rsrvd_0[26];
};

struct al_mod_srd_complex_regs {
	/* [0x0] */
	uint32_t rsrvd_0[64];
	/* [0x100] General configuration and status */
	struct al_mod_srd_complex_gen gen;
	/* [0x200] */
	uint32_t rsrvd_1[192];
	/* [0x500] Taranaki specific */
	struct al_mod_srd_complex_tnk_gen tnk_gen;
	/* [0x520] tpr_specific */
	struct al_mod_srd_complex_tpr_cfg tpr_cfg;
	/* [0x5a0] */
	uint32_t rsrvd_2[664];
	/* [0x1000] Access to SBM wrapper */
	uint32_t sbm_wrapper[1024];
	/* [0x2000] */
	uint32_t rsrvd_3[14336];
	/*
	 * [0x10000] Upto 64  SerDes 1024 rows each - (actual “Mem” rows are divided by 4 because of
	 * 32 bit width)
	 */
	uint32_t srd_wrapper[16384];
};


/*
 * Registers Fields
 */

/**** version register ****/
/*
 * Revision number (Minor)
 * Reset: 0x0         Access: RO
 */
#define SRD_COMPLEX_GEN_VERSION_RELEASE_NUM_MINOR_MASK 0x000000FF
#define SRD_COMPLEX_GEN_VERSION_RELEASE_NUM_MINOR_SHIFT 0
/*
 * Revision number (Major)
 * Reset: 0x0         Access: RO
 */
#define SRD_COMPLEX_GEN_VERSION_RELEASE_NUM_MAJOR_MASK 0x0000FF00
#define SRD_COMPLEX_GEN_VERSION_RELEASE_NUM_MAJOR_SHIFT 8
/*
 * date of release
 * Reset: 0x0         Access: RO
 */
#define SRD_COMPLEX_GEN_VERSION_DATE_DAY_MASK 0x001F0000
#define SRD_COMPLEX_GEN_VERSION_DATE_DAY_SHIFT 16
/*
 * month of release
 * Reset: 0x0         Access: RO
 */
#define SRD_COMPLEX_GEN_VERSION_DATA_MONTH_MASK 0x01E00000
#define SRD_COMPLEX_GEN_VERSION_DATA_MONTH_SHIFT 21
/*
 * year of release (starting from 2000)
 * Reset: 0x0         Access: RO
 */
#define SRD_COMPLEX_GEN_VERSION_DATE_YEAR_MASK 0x3E000000
#define SRD_COMPLEX_GEN_VERSION_DATE_YEAR_SHIFT 25
/*
 * Reserved
 * Reset: 0x0         Access: RO
 */
#define SRD_COMPLEX_GEN_VERSION_RESERVED_MASK 0xC0000000
#define SRD_COMPLEX_GEN_VERSION_RESERVED_SHIFT 30

/**** mux register ****/
/*
 * one hot mux select
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD0_MASK 0x00000003
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD0_SHIFT 0
/*
 * one hot mux select
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD1_MASK 0x00000030
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD1_SHIFT 4
/*
 * one hot mux select
 * Reset: 0x2         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD2_MASK 0x00000300
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD2_SHIFT 8
/*
 * one hot mux select
 * Reset: 0x2         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD3_MASK 0x00003000
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD3_SHIFT 12
/*
 * one hot mux select
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD4_MASK 0x00030000
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD4_SHIFT 16
/*
 * one hot mux select
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD5_MASK 0x00300000
#define SRD_COMPLEX_TNK_GEN_MUX_SELECT_OH_SRD5_SHIFT 20

/**** tx_clk register ****/
/*
 * tx clk select
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD0_MASK 0x00000003
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD0_SHIFT 0
/*
 * tx clk select
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD1_MASK 0x00000030
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD1_SHIFT 4
/*
 * tx clk select
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD2_MASK 0x00000300
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD2_SHIFT 8
/*
 * tx clk select
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD3_MASK 0x00003000
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD3_SHIFT 12
/*
 * tx clk select
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD4 (1 << 16)
/*
 * tx clk select
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TNK_GEN_TX_CLK_SELECT_SRD5 (1 << 20)

/**** main_sel register ****/
/*
 * Onehot mux: pcie4 ln0 connect to: 01 - srd4,  10- srd14
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN0_SELECT_OH_MASK 0x00000003
#define SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN0_SELECT_OH_SHIFT 0
/*
 * Onehot mux: pcie4 ln1 connect to: 01 - srd5,  10- srd15
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN1_SELECT_OH_MASK 0x00000030
#define SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN1_SELECT_OH_SHIFT 4

/**** main_mux register ****/
/*
 * Onehot mux: srd4 connect to: 01 - pcie0_ln4,  10- pcie4_ln0
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD4_MASK 0x00000003
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD4_SHIFT 0
/*
 * Onehot mux: srd5 connect to: 01 - pcie0_ln5,  10- pcie4_ln1
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD5_MASK 0x0000000C
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD5_SHIFT 2
/*
 * Onehot mux: srd6 connect to: 01 - pcie0_ln6,  10- pcie4_ln2
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD6_MASK 0x00000030
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD6_SHIFT 4
/*
 * Onehot mux: srd7 connect to: 01 - pcie0_ln7,  10- pcie4_ln3
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD7_MASK 0x000000C0
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD7_SHIFT 6
/*
 * Onehot mux: srd8 connect to: 01 - pcie0_ln8,  10- pcie1_ln0
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD8_MASK 0x00000300
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD8_SHIFT 8
/*
 * Onehot mux: srd9 connect to: 01 - pcie0_ln9,  10- pcie1_ln1
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD9_MASK 0x00000C00
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD9_SHIFT 10
/*
 * Onehot mux: srd10 connect to: 01 - pcie0_ln10,  10- pcie1_ln2
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD10_MASK 0x00003000
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD10_SHIFT 12
/*
 * Onehot mux: srd11 connect to: 01 - pcie0_ln11,  10- pcie1_ln3
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD11_MASK 0x0000C000
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD11_SHIFT 14
/*
 * Onehot mux: srd12 connect to: 001 - pcie0_ln12, 010 - pcie1_ln4, 100 - pcie5_ln0
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_MASK 0x00070000
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_SHIFT 16
/*
 * Onehot mux: srd13 connect to: 001 - pcie0_ln13, 010 - pcie1_ln5, 100 - pcie5_ln1
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_MASK 0x00700000
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_SHIFT 20
/*
 * Onehot mux: srd14 connect to: 0001 - pcie0_ln14, 0010 - pcie1_ln6, 0100 - pcie5_ln2
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_MASK 0x07000000
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_SHIFT 24
/*
 * Onehot mux: srd15 connect to: 0001 - pcie0_ln15, 0010 - pcie1_ln7, 0100 - pcie5_ln3
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_MASK 0x70000000
#define SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_SHIFT 28

/**** multi_sel register ****/
/*
 * Onehot mux: multi ln14 connect to: 001 - sata_d_2  010- eth0 100- eth0_external (other SBD)
 * Reset: 0x4         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN14_MASK 0x00000007
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN14_SHIFT 0
/*
 * Onehot mux: multi ln15 connect to: 001 - sata_d_3 ln  010- eth1 100- eth1_external (other SBD)
 * Reset: 0x4         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN15_MASK 0x00000070
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN15_SHIFT 4
/*
 * Onehot mux: eth_0 onnect to: 01 - ln14 10- external lane (30)
 * Reset: 0x2         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH0_MASK 0x00000300
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH0_SHIFT 8
/*
 * Onehot mux: eth_1 onnect to: 01 - ln15 10- external lane (31)
 * Reset: 0x2         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH1_MASK 0x00003000
#define SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH1_SHIFT 12

/**** reserv_mux register ****/
/* Reset: 0x1         Access: RW */
#define SRD_COMPLEX_TPR_CFG_RESERV_MUX_SELECT_OH_SPARE_MASK 0x0000FFFF
#define SRD_COMPLEX_TPR_CFG_RESERV_MUX_SELECT_OH_SPARE_SHIFT 0

/**** hs_sel register ****/
/*
 * 0 - tx0 is the clk source,  1 - tx1 is the source clk,  2- tx2 is the source clk,  3-tx3 is the
 * source clk
 * Reset: 0x0         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX0_CLKSEL_MASK 0x00000003
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX0_CLKSEL_SHIFT 0
/*
 * 0 - tx0 is the clk source,  1 - tx1 is the source clk,  2- tx2 is the source clk,  3-tx3 is the
 * source clk
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX1_CLKSEL_MASK 0x00000030
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX1_CLKSEL_SHIFT 4
/*
 * 0 - tx0 is the clk source,  1 - tx1 is the source clk,  2- tx2 is the source clk,  3-tx3 is the
 * source clk
 * Reset: 0x2         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX2_CLKSEL_MASK 0x00000300
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX2_CLKSEL_SHIFT 8
/*
 * 0 - tx0 is the clk source,  1 - tx1 is the source clk,  2- tx2 is the source clk,  3-tx3 is the
 * source clk
 * Reset: 0x3         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_MASK 0x00003000
#define SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_SHIFT 12

/**** hs_mux register ****/
/*
 * Select which ETH_V4 lane will connect to this SRDS - (one hot) 0001 - connect to ETHv4_ln0, 0010
 * - connect to ETHv4_ln1, 0100 - connect to ETHv4_ln2, 1000 - connect to ETHv4_ln3,
 * Reset: 0x1         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD0_MASK 0x0000000F
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD0_SHIFT 0
/*
 * Select which ETH_V4 lane will connect to this SRDS - (one hot) 0001 - connect to ETHv4_ln0, 0010
 * - connect to ETHv4_ln1, 0100 - connect to ETHv4_ln2, 1000 - connect to ETHv4_ln3,
 * Reset: 0x2         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD1_MASK 0x000000F0
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD1_SHIFT 4
/*
 * Select which ETH_V4 lane will connect to this SRDS - (one hot) 0001 - connect to ETHv4_ln0, 0010
 * - connect to ETHv4_ln1, 0100 - connect to ETHv4_ln2, 1000 - connect to ETHv4_ln3,
 * Reset: 0x4         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD2_MASK 0x00000F00
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD2_SHIFT 8
/*
 * Select which ETH_V4 lane will connect to this SRDS - (one hot) 0001 - connect to ETHv4_ln0, 0010
 * - connect to ETHv4_ln1, 0100 - connect to ETHv4_ln2, 1000 - connect to ETHv4_ln3,
 * Reset: 0x8         Access: RW
 */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD3_MASK 0x0000F000
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_SRD3_SHIFT 12
/* Reset: 0x1         Access: RW */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT0_MASK 0x000F0000
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT0_SHIFT 16
/* Reset: 0x2         Access: RW */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT1_MASK 0x00F00000
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT1_SHIFT 20
/* Reset: 0x4         Access: RW */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT2_MASK 0x0F000000
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT2_SHIFT 24
/* Reset: 0x8         Access: RW */
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT3_MASK 0xF0000000
#define SRD_COMPLEX_TPR_CFG_HS_MUX_SELECT_OH_CLIENT3_SHIFT 28

#ifdef __cplusplus
}
#endif

#endif

/** @} */
