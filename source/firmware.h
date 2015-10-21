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

const char* firmware_labels[][10] = {{"OLD 3DS", "NEW 3DS"}, {" 9", "10"}, {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}, {"0"}, {"USA", "EUR", "JPN"}};
const char* firmware_labels_url[][10] = {{"OLD", "NEW"}, {"9", "10"}, {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}, {"0"}, {"USA", "EUR", "JPN"}};
const int firmware_num_values[] = {2, 2, 10, 1, 3};
const int firmware_format_offsets[] = {3, 9, 11, 13, 16};
#define firmware_output_format "%s %s-%s-%s %s"
#define firmware_length 5

#endif
