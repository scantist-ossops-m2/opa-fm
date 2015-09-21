/* BEGIN_ICS_COPYRIGHT7 ****************************************

Copyright (c) 2015, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

** END_ICS_COPYRIGHT7   ****************************************/

/* [ICS VERSION STRING: unknown] */

#ifndef __IBA_STL_PM_H__
#define __IBA_STL_PM_H__

#include "iba/ib_pm.h"
#include "iba/stl_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

#include "iba/public/ipackon.h"

/*
 * Defines 
 */

#define STL_PM_CLASS_VERSION					0x80 	/* Performance Management version */

#define STL_PM_ATTRIB_ID_PORT_STATUS			0x40
#define STL_PM_ATTRIB_ID_CLEAR_PORT_STATUS		0x41
#define STL_PM_ATTRIB_ID_DATA_PORT_COUNTERS		0x42
#define STL_PM_ATTRIB_ID_ERROR_PORT_COUNTERS	0x43
#define STL_PM_ATTRIB_ID_ERROR_INFO				0x44

#define	MAX_PM_PORTS							49		/* max in STL Gen1 (ports 0-48 on PRR switch) */
#define	MAX_PM_VLS								16		/* max in STL Gen1 is 8 (0-7) plus VL15,*/

#define MAX_BIG_ERROR_COUNTERS					9
#define MAX_BIG_ERROR_COUNTERS_MIXED_MODE		5
#define MAX_BIG_VL_ERROR_COUNTERS				1

/* the default PM-PMA P_Key */
#define DEFAULT_PM_P_KEY						0x7fff

/* PortXmitData and PortRcvData are in units of flits (8 bytes) */
#define FLITS_PER_MiB ((uint64)1024*(uint64)1024/(uint64)8)
#define FLITS_PER_MB ((uint64)1000*(uint64)1000/(uint64)8)

/* MAD status codes for PM */
#define STL_PM_STATUS_REQUEST_TOO_LARGE			0x0100	/* response can't fit in a single MAD */
#define STL_PM_STATUS_NUMBLOCKS_INCONSISTENT	0x0200	/* attribute modifier number of blocks inconsistent with number of ports/blocks in MAD */
#define STL_PM_STATUS_OPERATION_FAILED			0x0300	/* an operation (such as clear counters) requested of the agent failed */

/* counter size mode values for attr mod bits 23-22 for STL Error Port Counters */
#define STL_PM_COUNTER_SIZE_MODE_ALL64			0
#define STL_PM_COUNTER_SIZE_MODE_ALL32			1
#define STL_PM_COUNTER_SIZE_MODE_MIXED			2

// bit is the bit number to set, mask is a 4*uint64 mask array
#define SETBITINMASK(bit, mask) (mask[3 - (bit / 64)] |= ((uint64)1 << (bit % 64)))

// portNums is a 255*int array, mask is a 4*uint64 mask array
// Note - should check that mask big enough
#define GET_PORTS_FROM_PORT_SELECT(portNums, mask) \
		{ int i, j; \
		  uint64 k = 0; \
		  for (i = 3; i >= 0; i--) { \
			 for (j = 0; j < 64; j++) \
				if (mask[i] & ((uint64)1 << j)) { \
								portNums[k++] = (64 * (3 - i)) + j; \
						} \
		  } \
		}


/* MAD structure definitions */

/* STL Port Counters - small request, large response */

typedef struct _STL_Port_Status_Req {
	uint8	PortNumber;
	uint8	Reserved[3];;
	uint32	VLSelectMask;
} PACK_SUFFIX STLPortStatusReq, STL_PORT_STATUS_REQ;

typedef struct _STL_Port_Status_Rsp {
	uint8	PortNumber;
	uint8	Reserved[3];
	uint32	VLSelectMask;
	uint64	PortXmitData;
	uint64	PortRcvData;
	uint64	PortXmitPkts;
	uint64	PortRcvPkts;
	uint64	PortMulticastXmitPkts;
	uint64	PortMulticastRcvPkts;
	uint64	PortXmitWait;
	uint64	SwPortCongestion;
	uint64	PortRcvFECN;
	uint64	PortRcvBECN;
	uint64	PortXmitTimeCong;
	uint64	PortXmitWastedBW;
	uint64	PortXmitWaitData;
		
	uint64	PortRcvBubble;
	uint64	PortMarkFECN;
	uint64	PortRcvConstraintErrors;
	uint64	PortRcvSwitchRelayErrors;
	uint64	PortXmitDiscards;
	uint64	PortXmitConstraintErrors;
	uint64	PortRcvRemotePhysicalErrors;
	uint64	LocalLinkIntegrityErrors;
	uint64	PortRcvErrors;
	uint64	ExcessiveBufferOverruns;
	uint64	FMConfigErrors;
	uint32	LinkErrorRecovery;
	uint32	LinkDowned;
	uint8	UncorrectableErrors;
	union {
		uint8	AsReg8;
		struct { IB_BITFIELD2(uint8,
				Reserved : 5,
				LinkQualityIndicator : 3)
		} PACK_SUFFIX s;
	} lq;
	uint8	Reserved2[6];
	struct _vls_pctrs {
		uint64	PortVLXmitData;
		uint64	PortVLRcvData;
		uint64	PortVLXmitPkts;
		uint64	PortVLRcvPkts;
		uint64	PortVLXmitWait;
		uint64	SwPortVLCongestion;
		uint64	PortVLRcvFECN;
		uint64	PortVLRcvBECN;
		uint64	PortVLXmitTimeCong;
		uint64	PortVLXmitWastedBW;
		uint64	PortVLXmitWaitData;
		uint64	PortVLRcvBubble;
		uint64	PortVLMarkFECN;
		uint64	PortVLXmitDiscards;
	} VLs[1]; /* n defined by number of bits in VLSelectmask */
} PACK_SUFFIX STLPortStatusRsp, STL_PORT_STATUS_RSP;

/* LinkQualityIndicator values */
#define STL_LINKQUALITY_EXCELLENT	5	/* working as intended */
#define STL_LINKQUALITY_VERY_GOOD	4	/* slightly below preferred, */
										/* no action needed */
#define STL_LINKQUALITY_GOOD		3	/* low end of acceptable, */
										/* recommend corrective action on */
										/* next maintenance window */
#define STL_LINKQUALITY_POOR		2	/* below acceptable, */
										/* recommend timely corrective action */
#define STL_LINKQUALITY_BAD			1	/* far below acceptable, */
										/* immediate corrective action */
#define STL_LINKQUALITY_NONE		0	/* link down */

typedef union {
	uint32	AsReg32;
	struct stl_counter_select_mask { IB_BITFIELD28(uint32,
		PortXmitData : 1,
		PortRcvData : 1,
		PortXmitPkts : 1,
		PortRcvPkts : 1,
		PortMulticastXmitPkts : 1,
		PortMulticastRcvPkts : 1,
		PortXmitWait : 1,
		SwPortCongestion : 1,
		PortRcvFECN : 1,
		PortRcvBECN : 1,
		PortXmitTimeCong : 1,
		PortXmitWastedBW : 1,
		PortXmitWaitData : 1,
		PortRcvBubble : 1,
		PortMarkFECN : 1,
		PortRcvConstraintErrors : 1,
		PortRcvSwitchRelayErrors : 1,
		PortXmitDiscards : 1,
		PortXmitConstraintErrors : 1,
		PortRcvRemotePhysicalErrors : 1,
		LocalLinkIntegrityErrors : 1,
		PortRcvErrors : 1,
		ExcessiveBufferOverruns : 1,
		FMConfigErrors : 1,
		LinkErrorRecovery : 1,
		LinkDowned : 1,
		UncorrectableErrors : 1,
		Reserved : 5)
	} PACK_SUFFIX s;
} CounterSelectMask_t;

typedef struct _STL_Clear_Port_Status {
	uint64	PortSelectMask[4];
	CounterSelectMask_t CounterSelectMask;
} PACK_SUFFIX STLClearPortStatus, STL_CLEAR_PORT_STATUS;

/* STL Data Port Counters - small request, bigger response */

typedef struct _STL_Data_Port_Counters_Req {
	uint64	PortSelectMask[4];				/* signifies for which ports the PMA is to respond */
	uint32	VLSelectMask;					/* signifies for which VLs the PMA is to respond */
} PACK_SUFFIX STLDataPortCountersReq, STL_DATA_PORT_COUNTERS_REQ;

struct _vls_dpctrs {
	uint64	PortVLXmitData;
	uint64	PortVLRcvData;
	uint64	PortVLXmitPkts;
	uint64	PortVLRcvPkts;
	uint64	PortVLXmitWait;
	uint64	SwPortVLCongestion;
	uint64	PortVLRcvFECN;
	uint64	PortVLRcvBECN;
	uint64	PortVLXmitTimeCong;
	uint64	PortVLXmitWastedBW;
	uint64	PortVLXmitWaitData;
	uint64	PortVLRcvBubble;
	uint64	PortVLMarkFECN;
};
 
struct _port_dpctrs {
	uint8	PortNumber;
	uint8	Reserved[3];
	union {
		uint32 AsReg32;
		struct { IB_BITFIELD2(uint32,
			Reserved : 29,
			LinkQualityIndicator : 3)
		} PACK_SUFFIX s;
	} lq;
	uint64	PortXmitData;
	uint64	PortRcvData;
	uint64	PortXmitPkts;
	uint64	PortRcvPkts;
	uint64	PortMulticastXmitPkts;
	uint64	PortMulticastRcvPkts;
	uint64	PortXmitWait;
	uint64	SwPortCongestion;
	uint64	PortRcvFECN;
	uint64	PortRcvBECN;
	uint64	PortXmitTimeCong;
	uint64	PortXmitWastedBW;
	uint64	PortXmitWaitData;
	uint64	PortRcvBubble;
	uint64	PortMarkFECN;
	uint64	PortErrorCounterSummary;		/* sum of all error counters for port */
	struct _vls_dpctrs VLs[1];							/* actual array size defined by number of bits in VLSelectmask */
};

typedef struct _STL_Data_Port_Counters_Rsp {
	uint64	PortSelectMask[4];
	uint32	VLSelectMask;
	uint32	Reserved;
	struct _port_dpctrs Port[1];								/* actual array size defined by number of ports in attribute modifier */
} PACK_SUFFIX STLDataPortCountersRsp, STL_DATA_PORT_COUNTERS_RSP;

/* STL Error Port Counters - small request, bigger response */

typedef struct _STL_Error_Port_Counters_Req {
	uint64	PortSelectMask[4];				/* signifies for which ports the PMA is to respond */
	uint32	VLSelectMask;					/* signifies for which VLs the PMA is to respond */
} PACK_SUFFIX STLErrorPortCountersReq, STL_ERROR_PORT_COUNTERS_REQ;

typedef struct _STL_Error_Port_Counters_Rsp {
	uint64	PortSelectMask[4];				/* echo from request */
	uint32	VLSelectMask;					/* echo from request */
	uint32	Reserved;
	struct _port_epctrs {
		uint8	PortNumber;
		uint8	Reserved[7];
		uint64	PortRcvConstraintErrors;	/* all counters are shown as 64-bit */
		uint64	PortRcvSwitchRelayErrors;
		uint64	PortXmitDiscards;
		uint64	PortXmitConstraintErrors;
		uint64	PortRcvRemotePhysicalErrors;
		uint64	LocalLinkIntegrityErrors;
		uint64	PortRcvErrors;
		uint64	ExcessiveBufferOverruns;
		uint64	FMConfigErrors;
		uint32	LinkErrorRecovery;			/* always 32-bit */
		uint32	LinkDowned;					/* always 32-bit */
		uint8	UncorrectableErrors;		/* always 8-bit */
		uint8	Reserved2[7];				/* may only need 3 reserve bytes if number of 32-bit ctrs is odd */
		struct _vls_epctrs {
			uint64	PortVLXmitDiscards;		/* all per-VL counters are shown as 64-bit */
		} VLs[1];							/* actual array size defined by number of bits in VLSelectmask */
	} Port[1];								/* actual array size defined by number of ports in attribute modifier */
} PACK_SUFFIX STLErrorPortCountersRsp, STL_ERROR_PORT_COUNTERS_RSP;

typedef struct _STL_Error_Info_Req {
	uint64	PortSelectMask[4];				/* signifies for which ports the PMA is to respond */
	union {
		uint32 AsReg32;
		struct { IB_BITFIELD8(uint32,
			PortRcvErrorInfo : 1,
			ExcessiveBufferOverrunInfo : 1,
			PortXmitConstraintErrorInfo : 1,
			PortRcvConstraintErrorInfo : 1,
			PortRcvSwitchRelayErrorInfo : 1,
			UncorrectableErrorInfo : 1,
			FMConfigErrorInfo : 1,
			Reserved : 25)
		} PACK_SUFFIX s;
	} ErrorInfoSelectMask;
	uint32	Reserved;

	struct {
		uint8	PortNumber;
		uint8	Reserved[7];

		/* PortRcvErrorInfo */
		struct {
			struct { IB_BITFIELD3(uint8,
				Status : 1,
				Reserved : 3,
				ErrorCode : 4)
			} PACK_SUFFIX s;
			union {
				uint8	AsReg8[17];			/* 136 bits of error info */
				struct {
					uint8	PacketFlit1[8];	/* first 64 bits of flit 1 */
					uint8	PacketFlit2[8];	/* first 64 bits of flit 2 */
					struct { IB_BITFIELD3(uint8,
						Flit1Bits : 1,		/* bit 64 of flit 1 */
						Flit2Bits : 1,		/* bit 64 of flit 2 */
						Reserved : 6)
					} PACK_SUFFIX s;
				} PACK_SUFFIX EI1to12;		/* error info for codes 1-12 */
				struct {
					uint8	PacketBytes[8];	/* first 64 bits of VL Marker flit */
					struct { IB_BITFIELD2(uint8,
						FlitBits : 1,		/* bit 64 of VL Marker flit */
						Reserved : 7)
					} PACK_SUFFIX s;
				} PACK_SUFFIX EI13;			/* error info for code 13 */
			} ErrorInfo;
			uint8 Reserved[6];
		} PACK_SUFFIX PortRcvErrorInfo;

		/* ExcessiveBufferOverrunInfo */
		struct {
			struct { IB_BITFIELD3(uint8,
				Status : 1,
				SC : 5,
				Reserved : 2)
			} PACK_SUFFIX s;
			uint8 Reserved[7];
		} PACK_SUFFIX ExcessiveBufferOverrunInfo;

		/* PortXmitConstraintErrorInfo */
		struct {
			struct { IB_BITFIELD2(uint8,
				Status : 1,
				Reserved : 7)
			} PACK_SUFFIX s;
			uint8	Reserved;
			uint16	P_Key;
			uint32	SLID;
		} PACK_SUFFIX PortXmitConstraintErrorInfo;

		/* PortRcvConstraintErrorInfo */
		struct {
			struct { IB_BITFIELD2(uint8,
				Status : 1,
				Reserved : 7)
			} PACK_SUFFIX s;
			uint8	Reserved;
			uint16	P_Key;
			uint32	SLID;
		} PACK_SUFFIX PortRcvConstraintErrorInfo;

		/* PortRcvSwitchRelayErrorInfo */
		struct {
			struct { IB_BITFIELD3(uint8,
				Status : 1,
				Reserved : 3,
				ErrorCode : 4)
			} PACK_SUFFIX s;
			uint8	Reserved[3];
			union {
				uint32	AsReg32;
				struct {
					uint32	DLID;
				} EI0; 						/* error code 0 */
				struct {
					uint8	EgressPortNum;
					uint8	Reserved[3];
				} EI2; 						/* error code 2 */
				struct {
					uint8	EgressPortNum;
					uint8	SC;
					uint16	Reserved;
				} EI3; 						/* error code 3 */
			} ErrorInfo;
		} PACK_SUFFIX PortRcvSwitchRelayErrorInfo;

		/* UncorrectableErrorInfo */
		struct {
			struct { IB_BITFIELD3(uint8,
				Status : 1,
				Reserved : 3,
				ErrorCode : 4)
			} PACK_SUFFIX s;
			uint8	Reserved;
		} PACK_SUFFIX UncorrectableErrorInfo;

		/* FMConfigErrorInfo */
		struct {
			struct { IB_BITFIELD3(uint8,
				Status : 1,
				Reserved : 3,
				ErrorCode : 4)
			} PACK_SUFFIX s;
			union {
				uint8	AsReg8;
				struct {
					uint8	VL;
				} EI0to2; 					/* error code 0-2 */
				struct {
					uint8	Distance;
				} EI3to5; 					/* error code 3-5 */
				struct {
					uint8	BadFlitBits;	/* bits [63:56] of bad packet */
				} EI6; 						/* error code 6 */
				struct {
					uint8	SC;
				} EI7; 						/* error code 7 */
			} ErrorInfo;
			uint32	Reserved;
		} PACK_SUFFIX FMConfigErrorInfo;

	} Port[1]; /* x defined by number of ports in attribute modifier */
} PACK_SUFFIX STLErrorInfoReq, STL_ERROR_INFO_REQ;

/* Swap routines */

static __inline
void
BSWAP_STL_PORT_STATUS_REQ(STL_PORT_STATUS_REQ *Dest)
{
#if CPU_LE
	Dest->VLSelectMask = ntoh32(Dest->VLSelectMask);
#endif
}

static __inline
void
BSWAP_STL_PORT_STATUS_RSP(STL_PORT_STATUS_RSP *Dest, boolean toNetwork)
{
#if CPU_LE
	int i, j;
	uint32 VLSelectMask;

	if (toNetwork)
		VLSelectMask = Dest->VLSelectMask;
	else
		VLSelectMask = ntoh32(Dest->VLSelectMask);

	Dest->VLSelectMask = ntoh32(Dest->VLSelectMask);
	Dest->PortXmitData = ntoh64(Dest->PortXmitData);
	Dest->PortRcvData = ntoh64(Dest->PortRcvData);
	Dest->PortXmitPkts = ntoh64(Dest->PortXmitPkts);
	Dest->PortRcvPkts = ntoh64(Dest->PortRcvPkts);
	Dest->PortMulticastXmitPkts = ntoh64(Dest->PortMulticastXmitPkts);
	Dest->PortMulticastRcvPkts = ntoh64(Dest->PortMulticastRcvPkts);
	Dest->PortXmitWait = ntoh64(Dest->PortXmitWait);
	Dest->SwPortCongestion = ntoh64(Dest->SwPortCongestion);
	Dest->PortRcvFECN = ntoh64(Dest->PortRcvFECN);
	Dest->PortRcvBECN = ntoh64(Dest->PortRcvBECN);
	Dest->PortXmitTimeCong = ntoh64(Dest->PortXmitTimeCong);
	Dest->PortXmitWastedBW = ntoh64(Dest->PortXmitWastedBW);
	Dest->PortXmitWaitData = ntoh64(Dest->PortXmitWaitData);
	Dest->PortRcvBubble = ntoh64(Dest->PortRcvBubble);
	Dest->PortMarkFECN = ntoh64(Dest->PortMarkFECN);
	Dest->PortRcvConstraintErrors = ntoh64(Dest->PortRcvConstraintErrors);
	Dest->PortRcvSwitchRelayErrors = ntoh64(Dest->PortRcvSwitchRelayErrors);
	Dest->PortXmitDiscards = ntoh64(Dest->PortXmitDiscards);
	Dest->PortXmitConstraintErrors = ntoh64(Dest->PortXmitConstraintErrors);
	Dest->PortRcvRemotePhysicalErrors = ntoh64(Dest->PortRcvRemotePhysicalErrors);
	Dest->LocalLinkIntegrityErrors = ntoh64(Dest->LocalLinkIntegrityErrors);
	Dest->PortRcvErrors = ntoh64(Dest->PortRcvErrors);
	Dest->ExcessiveBufferOverruns = ntoh64(Dest->ExcessiveBufferOverruns);
	Dest->FMConfigErrors = ntoh64(Dest->FMConfigErrors);
	Dest->LinkErrorRecovery = ntoh32(Dest->LinkErrorRecovery);
	Dest->LinkDowned = ntoh32(Dest->LinkDowned);
	/* Count the bits in the mask and process the VLs in succession */
	/* Assume that even though VL numbers may not be contiguous, that entries */
	/*   in the array are */
	for (i = 0, j = 0; i < MAX_PM_VLS; i++) {
		if ((VLSelectMask >> i) & (uint64)1) {
			Dest->VLs[j].PortVLXmitData = ntoh64(Dest->VLs[j].PortVLXmitData);
			Dest->VLs[j].PortVLRcvData = ntoh64(Dest->VLs[j].PortVLRcvData);
			Dest->VLs[j].PortVLXmitPkts = ntoh64(Dest->VLs[j].PortVLXmitPkts);
			Dest->VLs[j].PortVLRcvPkts = ntoh64(Dest->VLs[j].PortVLRcvPkts);
			Dest->VLs[j].PortVLXmitWait = ntoh64(Dest->VLs[j].PortVLXmitWait);
			Dest->VLs[j].SwPortVLCongestion = ntoh64(Dest->VLs[j].SwPortVLCongestion);
			Dest->VLs[j].PortVLRcvFECN = ntoh64(Dest->VLs[j].PortVLRcvFECN);
			Dest->VLs[j].PortVLRcvBECN = ntoh64(Dest->VLs[j].PortVLRcvBECN);
			Dest->VLs[j].PortVLXmitTimeCong = ntoh64(Dest->VLs[j].PortVLXmitTimeCong);
			Dest->VLs[j].PortVLXmitWastedBW = ntoh64(Dest->VLs[j].PortVLXmitWastedBW);
			Dest->VLs[j].PortVLXmitWaitData = ntoh64(Dest->VLs[j].PortVLXmitWaitData);
			Dest->VLs[j].PortVLRcvBubble = ntoh64(Dest->VLs[j].PortVLRcvBubble);
			Dest->VLs[j].PortVLMarkFECN = ntoh64(Dest->VLs[j].PortVLMarkFECN);
			Dest->VLs[j].PortVLXmitDiscards = ntoh64(Dest->VLs[j].PortVLXmitDiscards);
			j++;
		}
	}
#endif
}

static __inline
void
BSWAP_STL_CLEAR_PORT_STATUS_REQ(STL_CLEAR_PORT_STATUS *Dest)
{
#if CPU_LE
	int i;
	for (i = 0; i < 4; i++)
		Dest->PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	Dest->CounterSelectMask.AsReg32 = ntoh32(Dest->CounterSelectMask.AsReg32);
#endif
}

static __inline
void
BSWAP_STL_DATA_PORT_COUNTERS_REQ(STL_DATA_PORT_COUNTERS_REQ *Dest)
{
#if CPU_LE
	int i;
	for (i = 0; i < 4; i++)
		Dest->PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	Dest->VLSelectMask = ntoh32(Dest->VLSelectMask);
#endif
}

static __inline
void
BSWAP_STL_DATA_PORT_COUNTERS_RSP(STL_DATA_PORT_COUNTERS_RSP *Dest, boolean toNetwork)
{
#if CPU_LE
	int i, j, ii, jj;
	struct _port_dpctrs *port;
	uint64	PortSelectMask[4];
	uint32	VLSelectMask;

	if (toNetwork) {
		VLSelectMask = Dest->VLSelectMask;
		for (i = 0; i < 4; i++)
			PortSelectMask[i] = Dest->PortSelectMask[i];
	} else {
		VLSelectMask = ntoh32(Dest->VLSelectMask);
		for (i = 0; i < 4; i++)
			PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	}
	
	for (i = 0; i < 4; i++)
		Dest->PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	Dest->VLSelectMask = ntoh32(Dest->VLSelectMask);
	port = &(Dest->Port[0]);
	for (i = 3; i >= 0; i--) {
		for (j = 0; j < MAX_PM_PORTS; j++) {
			if ((PortSelectMask[i] >> j) & (uint64)1) {
				port->lq.AsReg32 = ntoh32(port->lq.AsReg32);
				port->PortXmitData = ntoh64(port->PortXmitData);
				port->PortRcvData = ntoh64(port->PortRcvData);
				port->PortXmitPkts = ntoh64(port->PortXmitPkts);
				port->PortRcvPkts = ntoh64(port->PortRcvPkts);
				port->PortMulticastXmitPkts = ntoh64(port->PortMulticastXmitPkts);
				port->PortMulticastRcvPkts = ntoh64(port->PortMulticastRcvPkts);
				port->PortXmitWait = ntoh64(port->PortXmitWait);
				port->SwPortCongestion = ntoh64(port->SwPortCongestion);
				port->PortRcvFECN = ntoh64(port->PortRcvFECN);
				port->PortRcvBECN = ntoh64(port->PortRcvBECN);
				port->PortXmitTimeCong = ntoh64(port->PortXmitTimeCong);
				port->PortXmitWastedBW = ntoh64(port->PortXmitWastedBW);
				port->PortXmitWaitData = ntoh64(port->PortXmitWaitData);
				port->PortRcvBubble = ntoh64(port->PortRcvBubble);
				port->PortMarkFECN = ntoh64(port->PortMarkFECN);
				port->PortErrorCounterSummary = ntoh64(port->PortErrorCounterSummary);
				for (ii = 0, jj = 0; ii < MAX_PM_VLS; ii++) {
					if ((VLSelectMask >> ii) & (uint64)1) {
						port->VLs[jj].PortVLXmitData = ntoh64(port->VLs[jj].PortVLXmitData);
						port->VLs[jj].PortVLRcvData = ntoh64(port->VLs[jj].PortVLRcvData);
						port->VLs[jj].PortVLXmitPkts = ntoh64(port->VLs[jj].PortVLXmitPkts);
						port->VLs[jj].PortVLRcvPkts = ntoh64(port->VLs[jj].PortVLRcvPkts);
						port->VLs[jj].PortVLXmitWait = ntoh64(port->VLs[jj].PortVLXmitWait);
						port->VLs[jj].SwPortVLCongestion = ntoh64(port->VLs[jj].SwPortVLCongestion);
						port->VLs[jj].PortVLRcvFECN = ntoh64(port->VLs[jj].PortVLRcvFECN);
						port->VLs[jj].PortVLRcvBECN = ntoh64(port->VLs[jj].PortVLRcvBECN);
						port->VLs[jj].PortVLXmitTimeCong = ntoh64(port->VLs[jj].PortVLXmitTimeCong);
						port->VLs[jj].PortVLXmitWastedBW = ntoh64(port->VLs[jj].PortVLXmitWastedBW);
						port->VLs[jj].PortVLXmitWaitData = ntoh64(port->VLs[jj].PortVLXmitWaitData);
						port->VLs[jj].PortVLRcvBubble = ntoh64(port->VLs[jj].PortVLRcvBubble);
						port->VLs[jj].PortVLMarkFECN = ntoh64(port->VLs[jj].PortVLMarkFECN);
						jj++;
					}
				}
				port = (struct _port_dpctrs *)&(port->VLs[jj]);
			}
		}
	}
#endif
}

static __inline
void
BSWAP_STL_ERROR_PORT_COUNTERS_REQ(STL_ERROR_PORT_COUNTERS_REQ *Dest)
{
#if CPU_LE
	int i;
	for (i = 0; i < 4; i++)
		Dest->PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	Dest->VLSelectMask = ntoh32(Dest->VLSelectMask);
#endif
}

static __inline
void
BSWAP_STL_ERROR_PORT_COUNTERS_RSP(STL_ERROR_PORT_COUNTERS_RSP *Dest, boolean toNetwork, uint32 counterSizeMode)
{
#if CPU_LE
	int i, j, ii;
	int counterCount1, counterCount2;
	int largeCounterCount;				/* need local copies of these 4, have to swap before using if coming from network */
	uint32 largeVLCounterCount;
	uint64 PortSelectMask[4];
	uint32 VLSelectMask;
	uint64 *p64;
	uint32 *p32;

	if (toNetwork) {
		for (i = 0; i < 4; i++)
			PortSelectMask[i] = Dest->PortSelectMask[i];
		VLSelectMask = Dest->VLSelectMask;
	} else {
		for (i = 0; i < 4; i++)
			PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
		VLSelectMask = ntoh32(Dest->VLSelectMask);
	}

	switch (counterSizeMode) {
		case STL_PM_COUNTER_SIZE_MODE_ALL64:
			largeCounterCount = MAX_BIG_ERROR_COUNTERS;
			largeVLCounterCount = MAX_BIG_VL_ERROR_COUNTERS;
			break;
		case STL_PM_COUNTER_SIZE_MODE_ALL32:
			largeCounterCount = 0;
			largeVLCounterCount = 0;
			break;
		case STL_PM_COUNTER_SIZE_MODE_MIXED:
			largeCounterCount = MAX_BIG_ERROR_COUNTERS_MIXED_MODE;
			largeVLCounterCount = 0;
			break;
		default:
			/* error - should never get here? */
			largeCounterCount = MAX_BIG_ERROR_COUNTERS;
			largeVLCounterCount = MAX_BIG_VL_ERROR_COUNTERS;
			break;
	}

	for (i = 0; i < 4; i++)
		Dest->PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	Dest->VLSelectMask = ntoh32(Dest->VLSelectMask);
	//ALL32 Mode does not have a 32bit Reserved field.
	p32 = (largeCounterCount ? (uint32 *)&Dest->Port[0].PortNumber : (uint32 *)&Dest->Reserved );
	/* Count the bits in the mask and process the ports in succession */
	/* Assume that even though port numbers may not be contiguous, that entries */
	/*   in the array are */
	for (i = 3; i >= 0; i--) {
		for (j = 0; j < MAX_PM_PORTS; j++) {
			if ((PortSelectMask[i] >> j) & (uint64)1) {
				p32 += (largeCounterCount ? 2 : 1); /* advance past PortNumber and 7 reserved bytes */
				p64 = (uint64 *)p32;
				for (counterCount1 = 0; counterCount1 < largeCounterCount; counterCount1++) {
					*p64 = ntoh64(*p64);
					p64++;
				}
				p32 = (uint32 *)p64;
				for (counterCount2 = counterCount1; counterCount2 < MAX_BIG_ERROR_COUNTERS; counterCount2++) {
					*p32 = ntoh32(*p32);
					p32++;
				}
				/* next, do two 32-bit swaps for LinkErrorRecovery and LinkDowned */
				*p32 = ntoh32(*p32);
				p32++;
				*p32 = ntoh32(*p32);
				p32++;
				p32 += (largeCounterCount ? 2 : 1); /* advance past UncorrectableErrors and 3 or 7 reserved bytes */
				p64 = (uint64 *)p32;
				for (ii = 0; ii < MAX_PM_VLS; ii++) {
					if ((VLSelectMask >> ii) & (uint64)1) {
						/* only one per-vl counter here, swap as necessary */
						if (largeVLCounterCount) {
							*p64 = ntoh64(*p64);
							p64++;
							p32 = (uint32 *)p64;
						} else {
							*p32 = ntoh32(*p32);
							p32 += (largeCounterCount ? 2 : 1); // MIXED mode has a 4 reserved bytes

							p64 = (uint64 *)p32;
						}
					}
				}
			}
		}
	}
#endif
}

static __inline
void
BSWAP_STL_ERROR_INFO(STL_ERROR_INFO_REQ *Dest, boolean toNetwork)
{
#if CPU_LE
	int i, j, k;
	//uint64 PortSelectMask[4];
	//if (toNetwork) {
	//	for (i = 0; i < 4; i++)
	//		PortSelectMask[i] = Dest->PortSelectMask[i];
	//} else {
	//	for (i = 0; i < 4; i++)
	//		PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	//}
	for (i = 0; i < 4; i++)
		Dest->PortSelectMask[i] = ntoh64(Dest->PortSelectMask[i]);
	Dest->ErrorInfoSelectMask.AsReg32 = ntoh32(Dest->ErrorInfoSelectMask.AsReg32);
	for (i = 3; i >= 0; i--) {
		for (j = 0, k = 0; j < 64; j++) {
			if ((Dest->PortSelectMask[i] >> j) & (uint64)1) {
				Dest->Port[k].PortXmitConstraintErrorInfo.P_Key = ntoh16(Dest->Port[k].PortXmitConstraintErrorInfo.P_Key);
				Dest->Port[k].PortXmitConstraintErrorInfo.SLID = ntoh32(Dest->Port[k].PortXmitConstraintErrorInfo.SLID);
				Dest->Port[k].PortRcvConstraintErrorInfo.P_Key = ntoh16(Dest->Port[k].PortRcvConstraintErrorInfo.P_Key);
				Dest->Port[k].PortRcvConstraintErrorInfo.SLID = ntoh32(Dest->Port[k].PortRcvConstraintErrorInfo.SLID);
				if (Dest->Port[k].PortRcvSwitchRelayErrorInfo.s.ErrorCode == 0)
					Dest->Port[k].PortRcvSwitchRelayErrorInfo.ErrorInfo.AsReg32 = ntoh32(Dest->Port[k].PortRcvSwitchRelayErrorInfo.ErrorInfo.AsReg32);
				k++;
			}
		}
	}
#endif
}

#if defined (__cplusplus)
};
#endif

#endif /* __IBA_STL_PM_H__ */