#ifndef FIRMWARE_H
#define FIRMWARE_H

typedef enum
{
	FW_MODEL_OLD = 0,
	FW_MODEL_NEW = 1,
}firmware_model_t;

typedef enum
{
	FW_REGION_USA = 0,
	FW_REGION_EUR = 1,
	FW_REGION_JPN = 2,
}firmware_region_t;

static char* getMenuVersion(int v[])
{
	static char* return_values[] = {"11272", "12288", "13330", "14336", "15360", "16404", "17415", "20480_usa", "19456"};

	if(v[2]==0 || v[2]==1)
		return return_values[0];
	else if(v[2]==2)
		return return_values[1];
	else if(v[2]==3)
		return return_values[2];
	else if(v[2]==4)
		return return_values[3];
	else if(v[2]==5)
		return return_values[4];
	else if(v[2]==6)
		return return_values[5];
	else if(v[2]==7)
		return return_values[6];
	else if(v[2]==9 && v[4]==FW_REGION_USA)
		return return_values[7];
	else return return_values[8];
}

static char* getMsetVersion(int v[])
{
	static char* return_values[] = {"8203", "9221"};
	if (v[2] < 6)
		return return_values[0];
	else
		return return_values[1];
}

static char* getRegion(int v[])
{
	static char* return_values[] = {"U", "E", "J"};
	return return_values[v[4]];
}

static char* getFirmVersion(int v[])
{
	static char* return_values[] = {"POST5", "N3DS"};
	return return_values[v[0]];
}

static void getPayloadName(int v[], char* out)
{
	if(!out)return;
	sprintf(out, "%s_%s_%s_%s", getFirmVersion(v), getRegion(v), getMenuVersion(v), getMsetVersion(v));
}

const char* firmware_labels[][10] = {{"OLD 3DS", "NEW 3DS"}, {"9"}, {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}, {"0"}, {"USA", "EUR", "JPN"}};
const int firmware_num_values[] = {2, 1, 10, 1, 3};
const int firmware_format_offsets[] = {3, 8, 10, 12, 15};
#define firmware_output_format "%s %s-%s-%s %s"
#define firmware_length 5

#endif
