#include "InputInterpreter.h"

void multiplication(const std::string& input, int& i, std::vector<char>& operations, std::vector<std::vector<char>>& stringNumbers)
{
    operations.push_back(input[i]);
    stringNumbers.push_back(std::vector<char>());
    //z*z*z+c
    int valueMod = (i - 1) % 2;
    int index = i;
    i = 0;
    for (int j = index - 1; j < input.size(); j++)
    {
        int valueOrOperation = j % 2;

        if (input[j] != '*' && valueOrOperation != valueMod)
            break;
        else if (valueMod == valueOrOperation)
            stringNumbers.back().push_back(input[j]);
        i++;
    }
    i--;
}

//Converts input into operations and what variables were used
void interpret(const std::string& input, std::vector<char>& operations, std::vector<std::vector<char>>& stringNumbers)
{
    for (int i = 0; i < input.size(); i++)
    {
        if (input[i] == '+' || input[i] == '-')
        {
            if (input.size() > i + 2 && input[i + 2] == '*')
            {
                //This part is still an issue, working on a fix
                i += 2;
                multiplication(input, i, operations, stringNumbers);
                i += 2;
                continue;
            }
            operations.push_back(input[i]);
            stringNumbers.push_back(std::vector<char>());

            if (stringNumbers.size() == 1)
            {
                stringNumbers.back().push_back(input[i - 1]);
            }
            else
            {
                stringNumbers.back().push_back(input[i + 1]);
                i++;
            }
        }
        else if (input[i] == '*')
        {
            multiplication(input, i, operations, stringNumbers);
        }

    }
}

void additionOrSubtraction(const std::vector<char>& operations, const std::vector<std::vector<char>>& stringNumbers, int index, std::complex<double>& finalValue, const std::complex<double> z, const std::complex<double> c)
{
    double factor = 1;
    if (operations[index] == '-')
        factor = -1;

    if (stringNumbers[index][0] == 'z')
        finalValue += factor * z;
    else
        finalValue += factor * c;

    if (stringNumbers[index].size() == 2)
    {
        if (stringNumbers[index][1] == 'z')
            finalValue += factor * z;
        else
            finalValue += factor * c;
    }
}

void multiply(const std::vector<std::vector<char>>& stringNumbers, int index, std::complex<double>& finalValue, const std::complex<double> z, const std::complex<double> c)
{
    std::complex<double> tempComplex(0, 0);

    if (stringNumbers[index][0] == 'z')
        tempComplex = z;
    else
        tempComplex = c;

    //We can start on 1 since tempComplex already is the first number in the operation
    for (int i = 1; i < stringNumbers[index].size(); i++)
    {
        if (stringNumbers[index][i] == 'z')
            tempComplex *= z;
        else
            tempComplex *= c;

    }
    finalValue += tempComplex;
}

std::vector<int> InterpretAndCalculate(const std::string& input, fractalsetinfo* fractal)
{
    //Create the vector to store the iteration count
    std::vector<int> allIterations;
    allIterations.reserve(fractal->resWidth * fractal->resHeight);
    
    //These will help us interpret the input
    std::vector<char> operations;
    std::vector<std::vector<char>> stringNumbers;

    interpret(input, operations, stringNumbers);


    //I like defining stuff once before a big loop to have the code more clean, instead of constant accessing variables from the fractal struct
    double rV = fractal->rV;
    double iV = fractal->iV;
    double currentWidth = fractal->resWidth;
    double currentHeight = fractal->resHeight;
    int currentMaxIterations = fractal->maxIterations;
    bool swapZandC = fractal->swapZandC;

    for (uint32_t i = 0; i < currentWidth; i++)
    {
        for (uint32_t j = 0; j < currentHeight; j++)
        {
            //Pixel
            std::complex<double> c((double)j / currentWidth * 4.0 - 2.0, (double)i / currentHeight * 4.0 - 2.0);

            std::complex<double> z(rV, iV);

            if (swapZandC) {
                std::complex<double> temp = c;
                c = z;
                z = temp;
            }


            int iterations = 0;
            while (abs(z) < 2 && iterations < currentMaxIterations) 
            {
                //It is important that finalValue exists, and that it is reset it loop
                //This variable makes sure that z does not increase in value until the end of the loop
                std::complex<double> finalValue(0, 0);

                for (int k = 0; k < operations.size(); k++)
                {
                    if (operations[k] == '+' || operations[k] == '-')
                        additionOrSubtraction(operations, stringNumbers, k, finalValue, z, c);
                    else
                        multiply(stringNumbers, k, finalValue, z, c);
                }
                z = finalValue;
                iterations++;
            }

            allIterations.push_back(iterations);
        }
    }

    return allIterations;
}
