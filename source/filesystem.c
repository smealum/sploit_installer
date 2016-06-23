#include <string.h>
#include <stdio.h>

#include <3ds.h>

#include "filesystem.h"

static Handle fsHandle;
FS_Archive saveGameArchive, sdmcArchive;

Result filesystemInit(void)
{
	Result ret;
	FS_Path sdmcArchivePath, saveGameArchivePath;

        if (R_FAILED(ret = srvGetServiceHandleDirect(&fsHandle, "fs:USER"))) return ret;
        if (R_FAILED(ret = FSUSER_Initialize(fsHandle))) return ret;
   
        sdmcArchivePath = (FS_Path){PATH_EMPTY, 1, (u8*)""};
	if (R_FAILED(ret = FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, sdmcArchivePath))) return ret;

	disableHBLHandle();

	saveGameArchivePath = (FS_Path){PATH_EMPTY, 1, (u8*)""};
	ret = FSUSER_OpenArchive(&saveGameArchive, ARCHIVE_SAVEDATA, saveGameArchivePath);

	enableHBLHandle();

	return ret;
}

Result filesystemExit(void)
{
	Result ret = 0;

	ret = FSUSER_CloseArchive(saveGameArchive);
	ret |= FSUSER_CloseArchive(sdmcArchive); //Or-ing error so that if for some reason the first one fails but the second doesn't, we get something back at least.

        fsEndUseSession();

	return ret;
}

void enableHBLHandle(void)
{
	fsEndUseSession(); 
}

void disableHBLHandle(void)
{
	fsUseSession(fsHandle);
}


