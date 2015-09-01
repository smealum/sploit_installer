#ifndef FILESYSTEM_H
#define FILESYSTEM_H

extern Handle saveGameFsHandle, sdmcFsHandle;
extern FS_archive saveGameArchive, sdmcArchive;

Result filesystemInit();
Result filesystemExit();

#endif
