#ifndef FILESYSTEM_H
#define FILESYSTEM_H

extern FS_Archive saveGameArchive, sdmcArchive;

Result filesystemInit(void);
Result filesystemExit(void);

void enableHBLHandle(void);
void disableHBLHandle(void);

#endif
