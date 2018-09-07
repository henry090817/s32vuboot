/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 */

#include <stdint.h>

const uint8_t config_param[318] = {
	0x00/*DQS Config : enable config*/,
	0x00/*DQS Config*/,
	0x00/*DQS Config*/,
	0x00/*DQS Config*/,
	0x01/*Hold Delay : Data aligned with 2x serial flash half clock*/,
	0x00/*Half Speed Phase Selection*/,
	0x00/*Half Speed Delay Selection*/,
	0x00/*Reserved*/,
	0x00/*Serial Clock Configuration*/,
	0x00/*Serial Clock Configuration*/,
	0x00/*Serial Clock Configuration*/,
	0x00/*Serial Clock Configuration*/,
	0x00/*SoC Configuration*/,
	0x00/*SoC Configuration*/,
	0x00/*SoC Configuration*/,
	0x00/*SoC Configuration*/,
	0x00/*Reserved*/,
	0x00/*Reserved*/,
	0x00/*Reserved*/,
	0x00/*Reserved*/,
	0x00/*Chip Select hold time*/,
	0x00/*Chip Select hold time*/,
	0x00/*Chip Select hold time*/,
	0x00/*Chip Select hold time*/,
	0x00/*Chip Select setup time*/,
	0x00/*Chip Select setup time*/,
	0x00/*Chip Select setup time*/,
	0x00/*Chip Select setup time*/,
	0x00/*Serial Flash A1 size*/,
	0x00/*Serial Flash A1 size*/,
	0x00/*Serial Flash A1 size*/,
	0x40/*Serial Flash A1 size*/,
	0x00/*Serial Flash A2 size*/,
	0x00/*Serial Flash A2 size*/,
	0x00/*Serial Flash A2 size*/,
	0x00/*Serial Flash A2 size*/,
	0x00/*Serial Flash B1 size- IGNORED in RPC mode*/,
	0x00/*Serial Flash B1 size*/,
	0x00/*Serial Flash B1 size*/,
	0x00/*Serial Flash B1 size*/,
	0x00/*Serial Flash B2 size*/,
	0x00/*Serial Flash B2 size*/,
	0x00/*Serial Flash B2 size*/,
	0x00/*Serial Flash B2 size*/,
	0x03/*Serial Clock Frequency - 80 MHz*/,
	0x00/*Serial Clock Frequency*/,
	0x00/*Serial Clock Frequency*/,
	0x00/*Serial Clock Frequency*/,
	0x00/*Reserved*/,
	0x00/*Reserved*/,
	0x00/*Reserved*/,
	0x00/*Reserved*/,
	0x00/*Mode of operation- IGNORED in RPC mode*/,
	0x00/*Serial Flash Port B Selection- IGNORED in RPC mode*/,
	0x01/*Dual Data Rate mode - Always enabled in RPC mode*/,
	0x01/*Data Strobe Signal - Always enabled in RPC mode*/,
	0x00/*Parallel Mode enable - IGNORED in RPC mode*/,
	0x00/*CS1 on Port A*/,
	0x00/*CS1 on Port B*/,
	0x00/*Full Speed Phase Selection*/,
	0x00/*Full Speed Delay Selection*/,
	0x00/*DDR Sampling Point, No need because DQS enabled*/,
	/*Flash specific LUT */
	0xA0,0x47,0x18,0x2B,0x10,0x4F,0x0F,0x0F,0x80,
	/* 128 bytes*/
	0x3B,0x00,0x03,
	/*STOP - 8pads*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

