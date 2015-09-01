#include <string.h>
#include <stdio.h>

#include <3ds.h>

#include "filesystem.h"

Handle saveGameFsHandle, sdmcFsHandle;
FS_archive saveGameArchive, sdmcArchive;

// bypass handle list
Result _srvGetServiceHandle(Handle* out, const char* name)
{
	Result rc = 0;

	u32* cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x50100;
	strcpy((char*) &cmdbuf[1], name);
	cmdbuf[3] = strlen(name);
	cmdbuf[4] = 0x0;
	
	if((rc = svcSendSyncRequest(*srvGetSessionHandle())))return rc;

	*out = cmdbuf[3];
	return cmdbuf[1];
}

Result filesystemInit()
{
	Result ret;
	
	ret = _srvGetServiceHandle(&saveGameFsHandle, "fs:USER");
	if(ret)return ret;
	
	ret = FSUSER_Initialize(&saveGameFsHandle);
	if(ret)return ret;

	ret = srvGetServiceHandle(&sdmcFsHandle, "fs:USER");
	if(ret)return ret;

	saveGameArchive = (FS_archive){0x00000004, (FS_path){PATH_EMPTY, 1, (u8*)""}};
	ret = FSUSER_OpenArchive(&saveGameFsHandle, &saveGameArchive);

	sdmcArchive = (FS_archive){0x00000009, (FS_path){PATH_EMPTY, 1, (u8*)""}};
	ret = FSUSER_OpenArchive(&sdmcFsHandle, &sdmcArchive);

	return ret;
}

Result filesystemExit()
{
	Result ret;
	
	ret = FSUSER_CloseArchive(&saveGameFsHandle, &saveGameArchive);
	ret = FSUSER_CloseArchive(&sdmcFsHandle, &sdmcArchive);
	ret = svcCloseHandle(saveGameFsHandle);
	ret = svcCloseHandle(sdmcFsHandle);

	return ret;
}
