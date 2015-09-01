#ifndef SAVEGAMEDATA_H
#define SAVEGAMEDATA_H

#include "Data0_10_NEW_bin.h"
#include "Data0_10_OLD_bin.h"
#include "Data0_10_eu_NEW_bin.h"
#include "Data0_10_eu_OLD_bin.h"
#include "Data0_11_NEW_bin.h"
#include "Data0_11_OLD_bin.h"
#include "Data0_11_eu_NEW_bin.h"
#include "Data0_11_eu_OLD_bin.h"
#include "Data1_10_NEW_bin.h"
#include "Data1_10_OLD_bin.h"
#include "Data1_10_eu_NEW_bin.h"
#include "Data1_10_eu_OLD_bin.h"
#include "Data1_11_NEW_bin.h"
#include "Data1_11_OLD_bin.h"
#include "Data1_11_eu_NEW_bin.h"
#include "Data1_11_eu_OLD_bin.h"
#include "Data2_10_NEW_bin.h"
#include "Data2_10_OLD_bin.h"
#include "Data2_10_eu_NEW_bin.h"
#include "Data2_10_eu_OLD_bin.h"
#include "Data2_11_NEW_bin.h"
#include "Data2_11_OLD_bin.h"
#include "Data2_11_eu_NEW_bin.h"
#include "Data2_11_eu_OLD_bin.h"

const u8* savegame_data[3][4][2] = {
	{
		{
			Data0_10_NEW_bin,
			Data0_10_OLD_bin,
		},
		{
			Data0_10_eu_NEW_bin,
			Data0_10_eu_OLD_bin,
		},
		{
			Data0_11_NEW_bin,
			Data0_11_OLD_bin,
		},
		{
			Data0_11_eu_NEW_bin,
			Data0_11_eu_OLD_bin,
		}
	},
	{
		{
			Data1_10_NEW_bin,
			Data1_10_OLD_bin,
		},
		{
			Data1_10_eu_NEW_bin,
			Data1_10_eu_OLD_bin,
		},
		{
			Data1_11_NEW_bin,
			Data1_11_OLD_bin,
		},
		{
			Data1_11_eu_NEW_bin,
			Data1_11_eu_OLD_bin,
		}
	},
	{
		{
			Data2_10_NEW_bin,
			Data2_10_OLD_bin,
		},
		{
			Data2_10_eu_NEW_bin,
			Data2_10_eu_OLD_bin,
		},
		{
			Data2_11_NEW_bin,
			Data2_11_OLD_bin,
		},
		{
			Data2_11_eu_NEW_bin,
			Data2_11_eu_OLD_bin,
		}
	}
};

static u8* getSavegameData(int v[], int iron_version, int iron_slot)
{
	return (u8*)savegame_data[iron_slot][iron_version * 2 + (v[4] == FW_REGION_EUR)][!v[0]];
}

#endif
