#pragma once

#include <iostream>
#include <vector>

struct zoom
{
	uint32_t zoomValue = 1;
	float x = 0;
	float y = 0;
};

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
	bool swapZandC = false;

	float rV = 0;
	float iV = 0;


	float offsetX = 0;//-0.750222f;
	float offsetY = 0;//0.031161f;

	int set = 0;

	std::string currentFunction = "z*z+c";
	int doNewFractal = 0;

	std::vector<zoom> zooming;
};

//Add something that can keep track of all the zooming done and navigate back and forth
//std::vector that contains width, height and offset, last member in the array is the current zoom