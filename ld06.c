/*
 * ld06.c
 *
 *  Created on: May 23, 2023
 *      Author: ardakk
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ld06.h"

uint8_t CalCRC8(uint8_t package[], uint8_t len)
{
	uint8_t crc = 0;
	uint16_t i;
	for (i = 0; i < len; i++)
	{
		crc = CrcTable[(crc ^ package[i]) & 0xff];
	}
	return crc;
}

LiDARFrameTypeDef AssignValues(uint8_t package[]){

	LiDARFrameTypeDef lf = {0};
	uint32_t distance_average = 0;
	uint8_t distance_count = 0;

	lf.header = package[0];

	lf.ver_len = package[1];

	lf.speed = (package[3] << 8 | package[2]);

	lf.start_angle = (package[5] << 8 | package[4]) / 100;

	lf.end_angle = (package[43] << 8 | package[42])/100; //adding because end angle is 8 or 9 degrees

	lf.timestamp = (package[45] << 8 | package[44]);

	lf.crc8 = package[46];

	float angle_step = (lf.end_angle - lf.start_angle)/11;
	for(int i = 0; i < 12; i++){
		lf.point[i].angle = lf.start_angle + angle_step*(i);
		lf.point[i].distance = (package[8 + i * 3 - 1] << 8 | package[8 + i * 3 - 2]);
		lf.point[i].confidence = package[8 + i * 3];
		if (lf.point[i].confidence >= 180)
		{
			distance_average += lf.point[i].distance;
			distance_count++;

		}
	}
	return lf;
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//
//	if(lidar_data == 0x54 && data_buffer[0] == 0)
//	{
//		data_buffer[count] = lidar_data;
//		count++;
//	}
//	else if (count <= 46 && data_buffer[0] == 0x54)
//	{
//		data_buffer[count] = lidar_data;
//		count++;
//	}
//	return;
//}
