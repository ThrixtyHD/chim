#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include "label.h"
#include "boot.h"
#include "scene.h"

class DebugConsole
{
public:
	DebugConsole(SDL_State& state) : inputLabel(state), caretLabel(state) {}
	struct OutputElement
	{
		// type contains 3 types. 0 = normal, 1 = warning and 2 = error.
		int type = 0;
		Label label;
	};

	std::vector<std::string> outputHistory{};
	std::vector<std::string> history{};
	std::vector<Label>outputLabels;
	Label inputLabel;
	Label caretLabel;
	std::string inputText = "";
	float consoleSpeed = 1;
	int outputLines = 56;
	int outputLinesCurrent = outputLines; // this variable changes when animating the console. init with the same as outputLines.
	int outputIndexOffsetToScroll = 0;
	int fontSize = 8;
	int offset = 2;

	bool validCommand = false;
	bool scrollMode = false;
	
	int deleteLater = 0;
	int consoleTextX = 64;

	int outputNextIndex = 0;

	int historyIndex = 0;
	int historyMax = 32;
	int outputHistoryMax = 256;
	int historyCount = 0;

	int caretPosition = 0;
	float caretFlashTimer = 0.0f;

	int currentCommandArgument = 0;

	int backgroundColorR = 0;
	int backgroundColorG = 0;
	int backgroundColorB = 0;
	int backgroundColorA = 72;
	float backgroundPositionX = 0.0f;
	float backgroundPositionY = 0.0f;
	float backgroundSizeWidth = 0.0f; // automatically set by state's window width
	float backgroundSizeHeight = (float)offset + (float)outputLinesCurrent * (inputLabel.fontSize + (float)offset); // look crazy but its the same formula as the labels. padding + lines which is times 8 + offset.
	// position is where im at in the string.
	int position = 0;

	// create a timer for animating in ms
	double consoleMovementTimer = 0.0;
	double consoleMovementTimerTarget = 1024.0;

	bool active = false;
	bool scrollingHistory = false;

	SDL_FRect backgroundTransform
	{
		backgroundPositionX,
		backgroundPositionY,
		backgroundSizeWidth, // automatically set by state's window width
		backgroundSizeHeight,
	};

	enum CONSOLE_STATE
	{
		CONSOLE_STATE_STATIONARY,
		CONSOLE_STATE_TRANSITION_IN,
		CONSOLE_STATE_TRANSITION_OUT,
	};

	CONSOLE_STATE consoleState = CONSOLE_STATE_STATIONARY;

	

	void ToggleConsole();
	void ActivateConsole();
	void DeactivateConsole();
	void AnimateConsole(SDL_State& state, double deltaTime);
	void GenerateInputLabel(SDL_State& state);
	void GenerateCaretLabel(SDL_State& state);
	void GenerateOutputHistory(SDL_State& state);
	void GenerateLabelsForOutput(SDL_State& state);
	void GrabWindowSizeForConsole(SDL_State& state);
	void Initialise(SDL_State& state);
	void GenerateHistory();
	void RenderLabels(SDL_State& state);
	void GrabInputEvents(SDL_Event& event);
	void ProcessCommand(SDL_State& state);
	void UpdateInputLabelPosition(SDL_State& state);
	void CaretLabelAnimate(double deltaTime);
	void MovePosition(int direction);
	void Output(std::string output);
	void OutputWarning(std::string output);
	void OutputError(std::string output);
	void RemoveCharFromString();
	void AddCharToString(int input);
	void RenderBackground(SDL_State& state);
	void ScrollHistory(int direction);
	void SearchAllConsoleCommands();
	void RegisterCommand(std::string commandName, std::string commandDescription, int argumentCount, std::function<std::vector<std::string>(std::vector<std::string>)> function);
	void RegisterCommands();
	void ExecuteCommand(std::string commandName);
	void AutoComplete();
	void CaretLabelPosition();
	void ScrollOutput(int direction);
	void Update(SDL_State& state, double deltaTime);

	// my idea is search thrpugh every command in the vector for the matching name and call it.
	struct Command
	{
		std::string commandName;
		std::string commandDescription;
		int arguementCount;
		std::function<std::vector<std::string>(const std::vector<std::string>& arguments)> function;
	};

	// commands
	std::vector<Command> commands
	{
		
	};
};
