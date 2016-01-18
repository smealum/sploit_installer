#include <string.h>
#include <stdio.h>

#include <3ds.h>

#include "filesystem.h"

static Handle fsHandle;
FS_Archive saveGameArchive, sdmcArchive;

Result filesystemInit(void)
{
	Result ret;
        if (R_FAILED(ret = srvGetServiceHandleDirect(&fsHandle, "fs:USER"))) return ret;
        if (R_FAILED(ret = FSUSER_Initialize(fsHandle))) return ret;
    
        fsUseSession(fsHandle, false);

        sdmcArchive = (FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}, 0};
	if (R_FAILED(ret = FSUSER_OpenArchive(&sdmcArchive))) return ret;

	saveGameArchive = (FS_Archive){ARCHIVE_SAVEDATA, (FS_Path){PATH_EMPTY, 1, (u8*)""}, 0};
	if (R_FAILED(ret = FSUSER_OpenArchive(&saveGameArchive))) return ret;
	      
	return ret;
}

Result filesystemExit(void)
{
	Result ret = FSUSER_CloseArchive(&saveGameArchive);
        ret |= FSUSER_CloseArchive(&sdmcArchive); //Or-ing error so that if for some reason the first one fails but the second doesn't, we get something back at least.

	FSUSER_CloseArchive(&saveGameArchive);
	FSUSER_CloseArchive(&sdmcArchive);
        fsEndUseSession();

	return ret;
}

void enableHBLHandle(void)
{
	fsEndUseSession(); 
}

void disableHBLHandle(void)
{
	fsUseSession(fsHandle, false);
}


