#pragma once

#include <iostream>
#include <vector>

struct clr
{
	float colour[3]{1, 1, 1};
};

struct fractalsetinfo
{
	std::vector<uint8_t> pixels;

	std::vector<clr> colours;
	uint8_t nrOfColours = 0;
	int maxIterations = 100;

	float calcMandelTime = 0.f;
	float updateTextureTime = 0.f;

	uint32_t resWidth = 1024;
	uint32_t resHeight = 1024;
	uint8_t resolution = 5;		//Same as 1024x1024, base resolution

	bool updateTexture = false;
	bool newCalculation = false;

	float z1 = 0;
	float z2 = 0;

	int set = 0;
};

//Add something that can keep track of all the zooming done and navigate back and forth
//std::vector that contains width, height and offset, last member in the array is the current zoom