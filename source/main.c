#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "filesystem.h"
#include "blz.h"
#include "firmware.h"
#include "savegame_data.h"

char status[256];

Result FSUSER_ControlArchive(Handle handle, FS_archive archive)
{
	u32* cmdbuf=getThreadCommandBuffer();

	u32 b1 = 0, b2 = 0;

	cmdbuf[0]=0x080d0144;
	cmdbuf[1]=archive.handleLow;
	cmdbuf[2]=archive.handleHigh;
	cmdbuf[3]=0x0;
	cmdbuf[4]=0x1; //buffer1 size
	cmdbuf[5]=0x1; //buffer1 size
	cmdbuf[6]=0x1a;
	cmdbuf[7]=(u32)&b1;
	cmdbuf[8]=0x1c;
	cmdbuf[9]=(u32)&b2;
 
	Result ret=0;
	if((ret=svcSendSyncRequest(handle)))return ret;
 
	return cmdbuf[1];
}

Result write_savedata(char* path, u8* data, u32 size)
{
	if(!path || !data || !size)return -1;

	Handle outFileHandle;
	u32 bytesWritten;
	Result ret = 0;
	int fail = 0;

	ret = FSUSER_OpenFile(&saveGameFsHandle, &outFileHandle, saveGameArchive, FS_makePath(PATH_CHAR, path), FS_OPEN_CREATE | FS_OPEN_WRITE, FS_ATTRIBUTE_NONE);
	if(ret){fail = -8; goto writeFail;}

	ret = FSFILE_Write(outFileHandle, &bytesWritten, 0x0, data, size, 0x10001);
	if(ret){fail = -9; goto writeFail;}

	ret = FSFILE_Close(outFileHandle);
	if(ret){fail = -10; goto writeFail;}

	ret = FSUSER_ControlArchive(saveGameFsHandle, saveGameArchive);

	writeFail:
	if(fail)sprintf(status, "failed to write to file : %d\n     %08X %08X", fail, (unsigned int)ret, (unsigned int)bytesWritten);
	else sprintf(status, "successfully wrote to file !\n     %08X               ", (unsigned int)bytesWritten);

	return ret;
}

typedef enum
{
	STATE_NONE,
	STATE_INITIAL,
	STATE_SELECT_SLOT,
	STATE_SELECT_IRON_VERSION,
	STATE_SELECT_FIRMWARE,
	STATE_DOWNLOAD_PAYLOAD,
	STATE_COMPRESS_PAYLOAD,
	STATE_INSTALL_PAYLOAD,
	STATE_INSTALLED_PAYLOAD,
	STATE_ERROR,
}state_t;

Result http_download(httpcContext *context, u8** out_buf, u32* out_size)
{
	Result ret=0;
	u32 statuscode=0;
	u32 contentsize=0;
	u8 *buf;

	ret = httpcBeginRequest(context);
	if(ret!=0)return ret;

	ret = httpcGetResponseStatusCode(context, &statuscode, 0);
	if(ret!=0)return ret;

	if(statuscode!=200)return -2;

	ret=httpcGetDownloadSizeState(context, NULL, &contentsize);
	if(ret!=0)return ret;

	buf = (u8*)malloc(contentsize);
	if(buf==NULL)return -1;
	memset(buf, 0, contentsize);

	ret = httpcDownloadData(context, buf, contentsize, NULL);
	if(ret!=0)
	{
		free(buf);
		return ret;
	}

	if(out_size)*out_size = contentsize;
	if(out_buf)*out_buf = buf;
	else free(buf);

	return 0;
}

int main()
{
	httpcInit();

	gfxInitDefault();
	gfxSet3D(false);

	Result ret = filesystemInit();

	PrintConsole topConsole, bttmConsole;
	consoleInit(GFX_TOP, &topConsole);
	consoleInit(GFX_BOTTOM, &bttmConsole);

	consoleSelect(&topConsole);
	consoleClear();

	state_t current_state = STATE_NONE;
	state_t next_state = STATE_INITIAL;

	static char top_text[2048];
	top_text[0] = '\0';

	int selected_slot = 0;
	int selected_iron_version = 1;

	int firmware_version[firmware_length] = {0, 0, 9, 0, 0};
	int firmware_selected_value = 0;
	
	static char payload_name[256];
	u8* payload_buf = NULL;
	u32 payload_size = 0;

	while (aptMainLoop())
	{
		hidScanInput();
		if(hidKeysDown() & KEY_START)break;

		// transition function
		if(next_state != current_state)
		{
			switch(next_state)
			{
				case STATE_INITIAL:
					strcat(top_text, " Welcome to the ironhax installer ! Please proceedwith caution, as you might lose data if you don't.You may press START at any time to return to menu.\n                            Press A to continue.\n\n");
					break;
				case STATE_SELECT_SLOT:
					strcat(top_text, " Please select the savegame slot IRONHAX will be\ninstalled to. D-Pad to select, A to continue.\n");
					break;
				case STATE_SELECT_IRON_VERSION:
					strcat(top_text, "\n\n\n Please select the version of IRONFALL you have\ninstalled. D-Pad to select, A to continue.\n");
					break;
				case STATE_SELECT_FIRMWARE:
					strcat(top_text, "\n\n\n Please select your console's firmware version.\nOnly select NEW 3DS if you own a New 3DS (XL).\nD-Pad to select, A to continue.\n");
					break;
				case STATE_DOWNLOAD_PAYLOAD:
					getPayloadName(firmware_version, payload_name);
					sprintf(top_text, "%s\n\n\n Downloading payload... %s\n", top_text, payload_name);
					break;
				case STATE_COMPRESS_PAYLOAD:
					strcat(top_text, " Processing payload...\n");
					break;
				case STATE_INSTALL_PAYLOAD:
					strcat(top_text, " Installing payload...\n");
					break;
				case STATE_INSTALLED_PAYLOAD:
					strcat(top_text, " Done ! ironhax was successfully installed.");
					break;
				case STATE_ERROR:
					strcat(top_text, " Looks like something went wrong. :(\n");
					break;
				default:
					break;
			}
			current_state = next_state;
		}

		consoleSelect(&topConsole);
		printf("\x1b[0;%dHironhax installer", (50 - 17) / 2);
		printf("\x1b[1;%dHby smea\n\n\n", (50 - 7) / 2);
		printf(top_text);

		// state function
		switch(current_state)
		{
			case STATE_INITIAL:
				if(hidKeysDown() & KEY_A)next_state = STATE_SELECT_SLOT;
				break;
			case STATE_SELECT_SLOT:
				{
					if(hidKeysDown() & KEY_UP)selected_slot++;
					if(hidKeysDown() & KEY_DOWN)selected_slot--;
					if(hidKeysDown() & KEY_A)next_state = STATE_SELECT_IRON_VERSION;

					if(selected_slot < 0) selected_slot = 0;
					if(selected_slot > 2) selected_slot = 2;

					printf((selected_slot >= 2) ? "                                             \n" : "                                            ^\n");
					printf("                            Selected slot : %d  \n", selected_slot + 1);
					printf((!selected_slot) ? "                                             \n" : "                                            v\n");
				}
				break;
			case STATE_SELECT_IRON_VERSION:
				{
					if(hidKeysDown() & KEY_UP)selected_iron_version++;
					if(hidKeysDown() & KEY_DOWN)selected_iron_version--;
					if(hidKeysDown() & KEY_A)next_state = STATE_SELECT_FIRMWARE;

					if(selected_iron_version < 0) selected_iron_version = 0;
					if(selected_iron_version > 1) selected_iron_version = 1;

					printf((selected_iron_version >= 1) ? "                                             \n" : "                                             ^\n");
					printf("                Selected IRONFALL version : 1.%d  \n", selected_iron_version);
					printf((!selected_iron_version) ? "                                             \n" : "                                             v\n");
				}
				break;
			case STATE_SELECT_FIRMWARE:
				{
					if(hidKeysDown() & KEY_LEFT)firmware_selected_value--;
					if(hidKeysDown() & KEY_RIGHT)firmware_selected_value++;

					if(firmware_selected_value < 0) firmware_selected_value = 0;
					if(firmware_selected_value >= firmware_length) firmware_selected_value = firmware_length - 1;

					if(hidKeysDown() & KEY_UP)firmware_version[firmware_selected_value]++;
					if(hidKeysDown() & KEY_DOWN)firmware_version[firmware_selected_value]--;

					if(firmware_version[firmware_selected_value] < 0) firmware_version[firmware_selected_value] = 0;
					if(firmware_version[firmware_selected_value] >= firmware_num_values[firmware_selected_value]) firmware_version[firmware_selected_value] = firmware_num_values[firmware_selected_value] - 1;
					
					if(hidKeysDown() & KEY_A)next_state = STATE_DOWNLOAD_PAYLOAD;

					int offset = 28 + firmware_format_offsets[firmware_selected_value];
					printf((firmware_version[firmware_selected_value] < firmware_num_values[firmware_selected_value] - 1) ? "%*s^%*s" : "%*s-%*s", offset, " ", 50 - offset - 1, " ");
					printf("        Selected firmware : " "%s %s-%s-%s %s" "\n", firmware_labels[0][firmware_version[0]], firmware_labels[1][firmware_version[1]], firmware_labels[2][firmware_version[2]], firmware_labels[3][firmware_version[3]], firmware_labels[4][firmware_version[4]]);
					printf((firmware_version[firmware_selected_value] > 0) ? "%*sv%*s" : "%*s-%*s", offset, " ", 50 - offset - 1, " ");
				}
				break;
			case STATE_DOWNLOAD_PAYLOAD:
				{
					httpcContext context;
					static char url[512];
					sprintf(url, "http://smealum.github.io/ninjhax2/Pvl9iD2Im5/otherapp/%s.bin", payload_name);

					Result ret = httpcOpenContext(&context, url, 0);
					if(ret)
					{
						sprintf(status, "Failed to open http context\n    Error code : %08X", (unsigned int)ret);
						next_state = STATE_ERROR;
						break;
					}

					ret = http_download(&context, &payload_buf, &payload_size);
					if(ret)
					{
						sprintf(status, "Failed to download payload\n    Error code : %08X", (unsigned int)ret);
						next_state = STATE_ERROR;
						break;
					}

					next_state = STATE_COMPRESS_PAYLOAD;
				}
				break;
			case STATE_COMPRESS_PAYLOAD:
				payload_buf = BLZ_Code(payload_buf, payload_size, (unsigned int*)&payload_size, BLZ_NORMAL);
				next_state = STATE_INSTALL_PAYLOAD;
				break;
			case STATE_INSTALL_PAYLOAD:
				{
					static char filename[128];
					sprintf(filename, "/Data%d", selected_slot);
					Result ret = write_savedata(filename, getSavegameData(firmware_version, selected_iron_version, selected_slot), 0x2000);
					if(ret)
					{
						sprintf(status, "Failed to install %s.\n    Error code : %08X", filename, (unsigned int)ret);
						next_state = STATE_ERROR;
						break;
					}
				}

				{
					Result ret = write_savedata("/payload.bin", payload_buf, payload_size);
					if(ret)
					{
						sprintf(status, "Failed to install payload\n    Error code : %08X", (unsigned int)ret);
						next_state = STATE_ERROR;
						break;
					}

					next_state = STATE_INSTALLED_PAYLOAD;	
				}
				break;
			case STATE_INSTALLED_PAYLOAD:
				next_state = STATE_NONE;
				break;
			default:
				break;
		}

		consoleSelect(&bttmConsole);
		printf("\x1b[0;0H  Current status :\n    %s\n", status);

		gspWaitForVBlank();
	}

	filesystemExit();

	gfxExit();
	httpcExit();
	return 0;
}
