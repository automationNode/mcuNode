#ifndef _interpreter_h
#define _interpreter_h
#include <Arduino.h>
#include <analogWrite.h>
#include "_json.h"
#include "_spiffs.h"
#include "_wifi.h"
#include "_display.h"
#include "_json.h"

//#define DEBUG
//#define DEBUG_VERBOSE

DynamicJsonDocument variablesJson(1024 * 5);

DynamicJsonDocument instructionsJson(1024 * 15);
int instructionsSetupLength = 0;
int instructionsLoopLength = 0;

bool startup = true;
bool restart = false;

void interpreterPrint(String parameter)
{
    Serial.print(parameter);
}

int calculateInstructionsLength(DynamicJsonDocument instructions)
{
    int i = 0;
    while (true)
    {
        if (!instructions[i].is<JsonObject>() &&
            !instructions[i].is<String>() &&
            !instructions[i].is<float>() &&
            !instructions[i].is<double>() &&
            !instructions[i].is<bool>() &&
            !instructions[i].is<short>() &&
            !instructions[i].is<unsigned short>() &&
            !instructions[i].is<signed short>() &&
            !instructions[i].is<long>() &&
            !instructions[i].is<unsigned long>() &&
            !instructions[i].is<signed long>() &&
            !instructions[i].is<long long>() &&
            !instructions[i].is<unsigned long long>() &&
            !instructions[i].is<signed long long>() &&
            !instructions[i].is<int>() &&
            !instructions[i].is<unsigned int>() &&
            !instructions[i].is<signed int>())
        {
            break;
        }
        else
        {
            // Serial.println(instruction);
        }
        i++;
    }

    return i;
}

DynamicJsonDocument unparse(DynamicJsonDocument contentJson)
{

    if (contentJson["get"]["backendJson"] == true)
    {
        contentJson["return"] = true;
        contentJson["content"] = backendJson;
        return contentJson;
    }
    if (contentJson["get"]["programmingFile"] == true)
    {
        DynamicJsonDocument contentJson(1024 * 20);
        contentJson["return"] = true;
        contentJson["content"] = readFile("/programmingFile.js");
        return contentJson;
    }
    if (contentJson["set"]["programmingFile"] == true)
    {
        String content = contentJson["content"];
#ifdef DEBUG_VERBOSE
        Serial.println();
        Serial.println();
        Serial.println();
        Serial.println("--> Content: " + content);
#endif
        saveFile("/programmingFile.js", content);
        contentJson["return"] = true;
        return contentJson;
    }
    if (contentJson["set"]["wifi"]["configuration"] == true)
    {
        String client = contentJson["client"];
#ifdef DEBUG_VERBOSE
        Serial.println("--> contentJson[set][wifi][configuration]: " + client);
#endif
        backendJson["wifi"]["client"] = contentJson["client"];
        backendJson["wifi"]["accessPoint"]["activated"] = !contentJson["client"]["activated"];
        saveFile("/backend.json", jsonToString(backendJson));
        contentJson["return"] = true;
        restart = true;
        return contentJson;
    }
    if (contentJson["execute"] == true)
    {

        if (contentJson["instruction"]["action"] == "set")
        {
            String variable = contentJson["instruction"]["variable"];
            DynamicJsonDocument value = contentJson["instruction"]["value"];

            if (isObject(value))
            {
                DynamicJsonDocument instruction(1024 * 5);
                instruction["execute"] = true;
                instruction["instruction"] = value;
                variablesJson[variable] = unparse(instruction)["return"];
            }
            else if (isNumber(value) || isString(value))
            {
                variablesJson[variable] = value;
            }

            contentJson["return"] = true;
#ifdef DEBUG_VERBOSE
            Serial.println("-->variablesJson: " + jsonToString(variablesJson));
#endif
            return contentJson;
        }

        if (contentJson["instruction"]["action"] == "get")
        {
            String variable = contentJson["instruction"]["variable"];
            contentJson["return"] = variablesJson[variable];
            return contentJson;
        }

        if (contentJson["instruction"]["action"] == "add")
        {
            double outcome = 0;
            bool error = false;

            if (contentJson["instruction"]["parameters"].is<JsonArray>())
            {
                int instructionsLenght = calculateInstructionsLength(contentJson["instruction"]["parameters"]);
                for (int instructionNumber = 0; instructionNumber < instructionsLenght; instructionNumber++)
                {
                    if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                        if (parameter["action"].is<String>())
                        {
                            DynamicJsonDocument instruction(1024 * 5);
                            instruction["execute"] = true;
                            instruction["instruction"] = parameter;
                            DynamicJsonDocument result = unparse(instruction);
                            if (isNumber(result["return"]))
                            {
                                outcome += double(result["return"]);
                            }
                            else
                            {
                                interpreterPrint("Instruction: " + jsonToString(instruction) + " returned invalid value " + jsonToString(result["return"]) + "\n");
                                error = true;
                            }
                        }
                        else
                        {
                            interpreterPrint("Parameter " + jsonToString(parameter) + " does not have the \"action\" key for to return some value");
                            error = true;
                        }
                    }
                    else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        outcome += double(contentJson["instruction"]["parameters"][instructionNumber]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson["instruction"]) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                        error = true;
                    }
                }
            }
            if (!error)
            {
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = outcome;
                contentJson["return"] = outcome;
                return contentJson;
            }
        }

        if (contentJson["instruction"]["action"] == "substract")
        {
            String parameters = contentJson["instruction"]["parameters"];
            double outcome = 0;
            bool error = false;

            if (contentJson["instruction"]["parameters"].is<JsonArray>())
            {
                int instructionsLenght = calculateInstructionsLength(contentJson["instruction"]["parameters"]);
                for (int instructionNumber = 0; instructionNumber < instructionsLenght; instructionNumber++)
                {
                    if (instructionNumber == 0)
                    {
                        if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                            if (parameter["action"].is<String>())
                            {
                                DynamicJsonDocument instruction(1024 * 5);
                                instruction["execute"] = true;
                                instruction["instruction"] = parameter;
                                DynamicJsonDocument result = unparse(instruction);
                                if (isNumber(result["return"]))
                                {
                                    outcome += double(result["return"]);
                                }
                                else
                                {
                                    interpreterPrint("Instruction: " + jsonToString(instruction) + " returned invalid value " + jsonToString(result["return"]) + "\n");
                                    error = true;
                                }
                            }
                            else
                            {
                                interpreterPrint("Parameter " + jsonToString(parameter) + " does not have the \"action\" key for to return some value");
                                error = true;
                            }
                        }
                        else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            outcome += double(contentJson["instruction"]["parameters"][instructionNumber]);
                        }
                        else
                        {
                            interpreterPrint("Instruction: " + jsonToString(contentJson["instruction"]) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                            error = true;
                        }
                    }
                    else if (instructionNumber > 0)
                    {
                        if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                            if (parameter["action"].is<String>())
                            {
                                DynamicJsonDocument instruction(1024 * 5);
                                instruction["execute"] = true;
                                instruction["instruction"] = parameter;
                                DynamicJsonDocument result = unparse(instruction);
                                if (isNumber(result["return"]))
                                {
                                    outcome -= double(result["return"]);
                                }
                                else
                                {
                                    interpreterPrint("Instruction: " + jsonToString(instruction) + " returned invalid value " + jsonToString(result["return"]) + "\n");
                                    error = true;
                                }
                            }
                            else
                            {
                                interpreterPrint("Parameter " + jsonToString(parameter) + " does not have the \"action\" key for to return some value");
                                error = true;
                            }
                        }
                        else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            outcome -= double(contentJson["instruction"]["parameters"][instructionNumber]);
                        }
                        else
                        {
                            interpreterPrint("Instruction: " + jsonToString(contentJson["instruction"]) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                            error = true;
                        }
                    }
                }
            }
            if (!error)
            {
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = outcome;
                contentJson["return"] = outcome;
                return contentJson;
            }
        }

        if (contentJson["instruction"]["action"] == "multiply")
        {
            String parameters = contentJson["instruction"]["parameters"];
            double outcome = 1;
            bool error = false;

            if (contentJson["instruction"]["parameters"].is<JsonArray>())
            {
                int instructionsLenght = calculateInstructionsLength(contentJson["instruction"]["parameters"]);
                for (int instructionNumber = 0; instructionNumber < instructionsLenght; instructionNumber++)
                {
                    if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                        if (parameter["action"].is<String>())
                        {
                            DynamicJsonDocument instruction(1024 * 5);
                            instruction["execute"] = true;
                            instruction["instruction"] = parameter;
                            DynamicJsonDocument result = unparse(instruction);
                            if (isNumber(result["return"]))
                            {
                                Serial.println("-->Outcome = " + String(outcome));
                                outcome *= double(result["return"]);
                            }
                            else
                            {
                                interpreterPrint("Instruction: " + jsonToString(instruction) + " returned invalid value " + jsonToString(result["return"]) + "\n");
                                error = true;
                            }
                        }
                        else
                        {
                            interpreterPrint("Parameter " + jsonToString(parameter) + " does not have the \"action\" key for to return some value");
                            error = true;
                        }
                    }
                    else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        Serial.println("-->Outcome = " + String(outcome));
                        outcome *= double(contentJson["instruction"]["parameters"][instructionNumber]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson["instruction"]) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                        error = true;
                    }
                }
            }
            if (!error)
            {
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = outcome;
                contentJson["return"] = outcome;
                return contentJson;
            }
        }

        if (contentJson["instruction"]["action"] == "divide")
        {
            String parameters = contentJson["instruction"]["parameters"];
            double outcome = 0;
            bool error = false;

            if (contentJson["instruction"]["parameters"].is<JsonArray>())
            {
                int instructionsLenght = calculateInstructionsLength(contentJson["instruction"]["parameters"]);
                for (int instructionNumber = 0; instructionNumber < instructionsLenght; instructionNumber++)
                {
                    if (instructionNumber == 0)
                    {
                        if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                            if (parameter["action"].is<String>())
                            {
                                DynamicJsonDocument instruction(1024 * 5);
                                instruction["execute"] = true;
                                instruction["instruction"] = parameter;
                                DynamicJsonDocument result = unparse(instruction);
                                if (isNumber(result["return"]))
                                {
                                    outcome = double(result["return"]);
                                }
                                else
                                {
                                    interpreterPrint("Instruction: " + jsonToString(instruction) + " returned invalid value " + jsonToString(result["return"]) + "\n");
                                    error = true;
                                }
                            }
                            else
                            {
                                interpreterPrint("Parameter " + jsonToString(parameter) + " does not have the \"action\" key for to return some value");
                                error = true;
                            }
                        }
                        else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            outcome = double(contentJson["instruction"]["parameters"][instructionNumber]);
                        }
                        else
                        {
                            interpreterPrint("Instruction: " + jsonToString(contentJson["instruction"]) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                            error = true;
                        }
                    }
                    else if (instructionNumber > 0)
                    {
                        if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                            if (parameter["action"].is<String>())
                            {
                                DynamicJsonDocument instruction(1024 * 5);
                                instruction["execute"] = true;
                                instruction["instruction"] = parameter;
                                DynamicJsonDocument result = unparse(instruction);
                                if (isNumber(result["return"]))
                                {
                                    outcome /= double(result["return"]);
                                }
                                else
                                {
                                    interpreterPrint("Instruction: " + jsonToString(instruction) + " returned invalid value " + jsonToString(result["return"]) + "\n");
                                    error = true;
                                }
                            }
                            else
                            {
                                interpreterPrint("Parameter " + jsonToString(parameter) + " does not have the \"action\" key for to return some value");
                                error = true;
                            }
                        }
                        else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                        {
                            outcome /= double(contentJson["instruction"]["parameters"][instructionNumber]);
                        }
                        else
                        {
                            interpreterPrint("Instruction: " + jsonToString(contentJson["instruction"]) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                            error = true;
                        }
                    }
                }
            }
            if (!error)
            {
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = outcome;
                contentJson["return"] = outcome;
                return contentJson;
            }
        }

        if (contentJson["instruction"]["action"] == "power")
        {

            DynamicJsonDocument parameters(1024);

            // value handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["value"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["value"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            // power level handlers
            if (isObject(contentJson["instruction"]["parameters"][1]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][1];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isString(result["return"]))
                    {
                        parameters["power"] = result["return"];
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][1]))
            {
                parameters["power"] = contentJson["instruction"]["parameters"][1];
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][1]) + "\n");
            }

            if (isNumber(parameters["value"]) && isNumber(parameters["power"]))
            {
                double value = parameters["value"];
                double power = parameters["power"];

                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = pow(value, power);
                contentJson["return"] = pow(value, power);
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"]));
            }
        }

        if (contentJson["instruction"]["action"] == "root")
        {

            DynamicJsonDocument parameters(1024);

            // value handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["value"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["value"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            // root level handlers
            if (isObject(contentJson["instruction"]["parameters"][1]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][1];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isString(result["return"]))
                    {
                        parameters["root"] = result["return"];
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][1]))
            {
                parameters["root"] = contentJson["instruction"]["parameters"][1];
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][1]) + "\n");
            }

            if (isNumber(parameters["value"]) && isNumber(parameters["power"]))
            {
                double value = parameters["value"];
                double root = parameters["root"];

                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = pow(value, 1 / root);
                contentJson["return"] = pow(value, 1 / root);
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"]));
            }
        }

        if (contentJson["instruction"]["action"] == "print")
        {
            String parameters = contentJson["instruction"]["parameters"];
            if (contentJson["instruction"]["parameters"].is<JsonArray>())
            {
                int instructionsLenght = calculateInstructionsLength(contentJson["instruction"]["parameters"]);
                for (int instructionNumber = 0; instructionNumber < instructionsLenght; instructionNumber++)
                {
                    if (isObject(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][instructionNumber];
                        if (parameter["action"].is<String>())
                        {
                            DynamicJsonDocument instruction(1024 * 5);
                            instruction["execute"] = true;
                            instruction["instruction"] = parameter;
                            if (instructionNumber == (instructionsLenght - 1))
                            {
                                interpreterPrint(jsonToString(unparse(instruction)["return"]) + "\n");
                            }
                            else
                            {
                                interpreterPrint(jsonToString(unparse(instruction)["return"]));
                            }
                        }
                        else
                        {
                            if (instructionNumber == (instructionsLenght - 1))
                            {
                                interpreterPrint(jsonToString(parameter) + "\n");
                            }
                            else
                            {
                                interpreterPrint(jsonToString(parameter));
                            }
                        }
                    }
                    else if (isString(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        if (instructionNumber == (instructionsLenght - 1))
                        {
                            interpreterPrint(jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                        }
                        else
                        {
                            interpreterPrint(contentJson["instruction"]["parameters"][instructionNumber]);
                        }
                    }
                    else if (isNumber(contentJson["instruction"]["parameters"][instructionNumber]))
                    {
                        if (instructionNumber == (instructionsLenght - 1))
                        {
                            interpreterPrint(jsonToString(contentJson["instruction"]["parameters"][instructionNumber]) + "\n");
                        }
                        else
                        {
                            interpreterPrint(contentJson["instruction"]["parameters"][instructionNumber]);
                        }
                    }
                }
            }
        }

        if (contentJson["instruction"]["action"] == "wait")
        {
            // Time value handlers
            String parameters = contentJson["instruction"]["parameters"];
            if (contentJson["instruction"]["parameters"].is<JsonArray>())
            {
                if (isObject(contentJson["instruction"]["parameters"][0]))
                {
                    DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                    if (parameter["action"].is<String>())
                    {
                        DynamicJsonDocument instruction(1024 * 5);
                        instruction["execute"] = true;
                        instruction["instruction"] = parameter;
                        DynamicJsonDocument result = unparse(instruction);
                        if (isNumber(result["return"]))
                        {
                            delay(int(result["return"]));
                        }
                        else
                        {
                            interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                        }
                    }
                }
                else if (isNumber(contentJson["instruction"]["parameters"][0]))
                {
                    delay(int(contentJson["instruction"]["parameters"][0]));
                }
                else
                {
                    interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
                }
            }
            contentJson["return"] = true;
            return contentJson;
        }

        if (contentJson["instruction"]["action"] == "pinMode")
        {

            DynamicJsonDocument parameters(1024);

            // Gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["gpio"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["gpio"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            // State gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][1]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][1];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isString(result["return"]))
                    {
                        parameters["value"] = result["return"];
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isString(contentJson["instruction"]["parameters"][1]))
            {
                parameters["value"] = contentJson["instruction"]["parameters"][1];
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][1]) + "\n");
            }

            if (isNumber(parameters["gpio"]) && isString(parameters["value"]))
            {
                uint8_t gpio = parameters["gpio"];
                String value = parameters["value"];

                if (value == "OUTPUT")
                {
                    pinMode(gpio, OUTPUT);
                }
                else if (value == "INPUT")
                {
                    pinMode(gpio, INPUT);
                }
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = value;
                contentJson["return"] = value;
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"]));
            }
        }

        if (contentJson["instruction"]["action"] == "digitalWrite")
        {
            DynamicJsonDocument parameters(1024);

            // Gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["gpio"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["gpio"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            // State gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][1]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][1];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isString(result["return"]))
                    {
                        parameters["value"] = result["return"];
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isString(contentJson["instruction"]["parameters"][1]))
            {
                parameters["value"] = contentJson["instruction"]["parameters"][1];
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][1]) + "\n");
            }

            if (isNumber(parameters["gpio"]) && isString(parameters["value"]))
            {
                uint8_t gpio = parameters["gpio"];
                String value = parameters["value"];
                if (value == "HIGH")
                {
                    digitalWrite(gpio, HIGH);
                }
                else if (value == "LOW")
                {
                    digitalWrite(gpio, LOW);
                }
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = value;
                contentJson["return"] = value;
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }
        }
        if (contentJson["instruction"]["action"] == "analogWrite")
        {
            DynamicJsonDocument parameters(1024);

            // Gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["gpio"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["gpio"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            // State gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][1]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][1];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["value"] = result["return"];
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][1]))
            {
                parameters["value"] = int(contentJson["instruction"]["parameters"][1]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][1]) + "\n");
            }

            if (isNumber(parameters["gpio"]) && isNumber(parameters["value"]))
            {
                uint8_t gpio = parameters["gpio"];
                float value = parameters["value"];
                analogWrite(gpio, int32_t((value / 100) * 255));
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = value;
                contentJson["return"] = value;
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }
        }
        if (contentJson["instruction"]["action"] == "digitalRead")
        {

            DynamicJsonDocument parameters(1024);

            // Gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["gpio"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["gpio"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            if (isNumber(parameters["gpio"]))
            {
                int gpio = parameters["gpio"];
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = bool(digitalRead(gpio));
                contentJson["return"] = bool(digitalRead(gpio));
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }
        }
        if (contentJson["instruction"]["action"] == "analogRead")
        {
            DynamicJsonDocument parameters(1024);

            // Gpio handlers
            if (isObject(contentJson["instruction"]["parameters"][0]))
            {
                DynamicJsonDocument parameter = contentJson["instruction"]["parameters"][0];
                if (parameter["action"].is<String>())
                {
                    DynamicJsonDocument instruction(1024 * 5);
                    instruction["execute"] = true;
                    instruction["instruction"] = parameter;
                    DynamicJsonDocument result = unparse(instruction);
                    if (isNumber(result["return"]))
                    {
                        parameters["gpio"] = int(result["return"]);
                    }
                    else
                    {
                        interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(result["return"]));
                    }
                }
            }
            else if (isNumber(contentJson["instruction"]["parameters"][0]))
            {
                parameters["gpio"] = int(contentJson["instruction"]["parameters"][0]);
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }

            if (isNumber(parameters["gpio"]))
            {
                int gpio = parameters["gpio"];
                float lecture = float(analogRead(gpio));
                contentJson["instruction"]["executed"] = true;
                contentJson["instruction"]["return"] = int((lecture / 4095) * 100);
                contentJson["return"] = int((lecture / 4095) * 100);
                return contentJson;
            }
            else
            {
                interpreterPrint("Instruction: " + jsonToString(contentJson) + " has invalid parameter " + jsonToString(contentJson["instruction"]["parameters"][0]) + "\n");
            }
        }
        contentJson["return"] = false;
        return contentJson;
    }
    contentJson["return"] = NULL;
    return contentJson;
}

DynamicJsonDocument executeInstruction(DynamicJsonDocument instruction)
{
    DynamicJsonDocument contentJson(1024 * 5);
    contentJson["execute"] = true;
    contentJson["instruction"] = instruction;
    return unparse(contentJson);
}

void instructionsParse()
{
    if (startup)
    {
        String instructions = readFile("/instructions.json");
        Serial.println(instructions);
        instructionsJson = stringToJson(instructions);
        instructionsSetupLength = calculateInstructionsLength(instructionsJson["setup"]);
        instructionsLoopLength = calculateInstructionsLength(instructionsJson["loop"]);
        startup = false;
    }
}

void executeSetup()
{
#ifdef DEBUG
    Serial.println("--------------->Executing setup----------------------------->");
#endif
    for (int instructionNumber = 0; instructionNumber < instructionsSetupLength; instructionNumber++)
    {
        String instruction = jsonToString(instructionsJson["setup"][instructionNumber]);
        String instructionResult = jsonToString(executeInstruction(instructionsJson["setup"][instructionNumber]));
#ifdef DEBUG_VERBOSE
        Serial.println();
        Serial.println("-->Executing: " + instruction);
        Serial.println("-->Executed: " + instructionResult);
#endif
    }
}

void executeLoop()
{
#ifdef DEBUG
    Serial.println("--------------->Executing loop------------------------------->");
#endif
    for (int instructionNumber = 0; instructionNumber < instructionsLoopLength; instructionNumber++)
    {
#ifdef DEBUG_VERBOSE
        Serial.println();
        Serial.println("-->Executing: " + jsonToString(instructionsJson["loop"][instructionNumber]));
#endif

        String instructionResult = jsonToString(executeInstruction(instructionsJson["loop"][instructionNumber]));

#ifdef DEBUG_VERBOSE
        Serial.println("-->Executed: " + instructionResult);
#endif
    }
}

#endif