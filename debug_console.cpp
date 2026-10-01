#include "debug_console.h"
#include "label.h"
#include <assert.h>

void DebugConsole::ActivateConsole()
{
    // just to make sure, we set the outputLines to 0
    outputLinesCurrent = 0;
    active = true;
    consoleState = DebugConsole::CONSOLE_STATE_TRANSITION_IN;
}

void DebugConsole::ExecuteCommand(std::string input)
{
    
    if (input == "")
    {
        return;
    }


    std::vector<std::string> arguments;
    std::string currentInputSection = "";
    char characters = 0;

    // read each char and 
    for (char character : input)
    {
        currentInputSection += character;
        if (character == (' '))
        {
            // push everything it copied into the an arg.
            currentInputSection.erase(currentInputSection.end() - 1);
            arguments.push_back(currentInputSection);                  // this means there are # of parts in this command. part[0] is the command. parts[>0] are the values.
            // refresh for the next section.
            currentInputSection.clear();
            
        }
        
        //std::cout << "[DebugConsole::ParsedArguments()] Current string: '" << currentInputSection << "'\n";
    }
    if (!currentInputSection.empty())
    {
        arguments.push_back(currentInputSection);
        currentInputSection.clear();
    }
    //std::cout << "[DebugConsole::ParsedArguments()] Final evaluation of arguments (Num): '" << arguments.size() << "'\n";
    //std::cout << "[DebugConsole::ParsedArguments()] If arguments is 0, something went wrong: '" << arguments.size() << "'\n";

    for (int i = 0; i < arguments.size(); i++)
    {
        //std::cout << "[DebugConsole::ParsedArguments()] Arguments: '" << arguments[i] << "'\n";
    }
    for (int i = 0; i < commands.size(); i++) // for each command, verify integrity
    {
        if (arguments[0] == commands[i].commandName)
        {
            if (arguments.size() - 1 == commands[i].arguementCount)
            {
                //std::cout << "[DebugConsole::ParsedArguments()] Arguments 0 is: '" << arguments[0] << "' which is included in '" << inputText << "'\n";
                validCommand = true;
                scrollMode = false;
                outputIndexOffsetToScroll = 0;
                historyIndex = 0;
                commands[i].function(arguments);
                break;
            }
            else if (arguments.size() - 1 > commands[i].arguementCount)
            {
                Output("Error: Too many parameters.");
                validCommand = false;
            }
            else if (arguments.size() - 1 < commands[i].arguementCount)
            {
                Output("Error: Not enough parameters.");
                validCommand = false;
            }
        }
        else
        {
            // theres some sort of issue going on where this calls more than it should. remember to look into it in the future.
            // were setting a bool so it visually isnt an issue but i imagine it could affect performance or allow invalid commands.
            validCommand = false;
        }
    }
    
    if (!validCommand)
    {
        Output("Unknown command: '" + inputText + "'");
    }

    // clean up the input in the console.
    if (validCommand)
    {
        if (history.size() < historyMax)
        {
            history.push_back(input);
            // export the vector into a file
            std::ofstream fileOutput("console_history.txt");
            for (int i = 0; i < history.size(); i++)
            {
                std::cout << i << ": '" << history[i] << "'\n";
                std::string command = history[i] + "\n";
                fileOutput << command;
            }
            fileOutput.close();
        }
        // handle dealing with the hisotry
        else if (history.size() >= historyMax)
        {
            history.erase(history.begin()); // remember in the future, this automatically reorders for u.
            history.push_back(input);
            // export the vector into a file
            std::ofstream fileOutput("console_history.txt");
            for (int i = 0; i < history.size(); i++)
            {
                std::cout << i << ": '" << history[i] << "'\n";
                std::string command = history[i] + "\n";
                fileOutput << command;
            }
            fileOutput.close();
        }
        

        inputLabel.Clear();
        inputText = "";
        inputLabel.text = "";
        inputLabel.SetText(inputText);
        MovePosition(2);
    }
}

void DebugConsole::RegisterCommand(std::string commandName, std::string commandDescription, int argumentCount, std::function<std::vector<std::string>(std::vector<std::string>)> function)
{
    Command newCommand = {commandName, commandDescription, argumentCount ,function};
    for (int i = 0; i < commands.size(); i++)
    {
        if (commandName == commands[i].commandName)
        {
            Output("Error: command '" + newCommand.commandName + "' already exists.");
            return;
        }
    }
    commands.push_back(newCommand);
    std::cout << "[DebugConsole::RegisterCommand] Registered a new command:\nName: '" << commandName << "'\nDescription: '" << commandDescription << "'\nArguments: '" << argumentCount << "'\n\n";
}



void DebugConsole::SearchAllConsoleCommands()
{
    for (int i = 0; i < commands.size(); i++)
    {
        std::string printCommands = commands[i].commandName + " - " + commands[i].commandDescription + "\n";
        Output(printCommands);
    }
}

void DebugConsole::ToggleConsole()
{
    // start transitioning in the console also rendering everything.
    if (!active && consoleState == DebugConsole::CONSOLE_STATE_STATIONARY)
    {
        ActivateConsole();
    }
    // dont set active tot false cause we want to see the console close.
    else if (active && consoleState == DebugConsole::CONSOLE_STATE_STATIONARY)
    {
        DeactivateConsole();
    }
    //std::cout << consoleState << "\n";
}

void DebugConsole::DeactivateConsole()
{
    active = true;
    consoleState = DebugConsole::CONSOLE_STATE_TRANSITION_OUT;
}

// ran every frame.
void DebugConsole::AnimateConsole(SDL_State& state, double delta)
{
    if (active && consoleState == DebugConsole::CONSOLE_STATE_TRANSITION_OUT)
    {
        consoleMovementTimer += 1.0 * delta;
        if (consoleMovementTimer >= 0.01)
        {
            consoleMovementTimer = 0;
            if (outputLinesCurrent > -1)
            {
                outputLinesCurrent--;
                if (outputLinesCurrent == 0)
                {
                    active = false; 
                    consoleState = DebugConsole::CONSOLE_STATE_STATIONARY;
                }
            }
        }
    }
    if (active && consoleState == DebugConsole::CONSOLE_STATE_TRANSITION_IN)
    {
        consoleMovementTimer += 1.0 * delta;
        if (consoleMovementTimer >= 0.01)
        {
            consoleMovementTimer = 0;
            if (outputLinesCurrent < outputLines)
            {
                outputLinesCurrent++;
                if (outputLinesCurrent == outputLines)
                {
                    consoleState = DebugConsole::CONSOLE_STATE_STATIONARY;
                }
            }

        }
    }
    deleteLater = (outputLinesCurrent - 1) * (fontSize + offset) - ((outputLines - 1) * (fontSize + offset) - 4); // where the fuck is 306 coming from??? i brute forced this but i cant see how a calc lands here.

}

void DebugConsole::GenerateInputLabel(SDL_State& state)
{
    Label newInputLabel = inputLabel;
    inputLabel.SetFont();
    inputLabel.fontSize = fontSize;
    inputLabel.labelType = Label::LABEL_TYPE_NORMAL;
    inputLabel.SetColorEnum(Label::LABEL_COLOR_WHITE);
    std::cout << "[Console::GenerateInputLabel] Successfully generated the input label.\n";
}

void DebugConsole::GenerateCaretLabel(SDL_State& state)
{
    Label newCaretLabel = caretLabel;
    caretLabel.SetFont();
    caretLabel.fontSize = fontSize;
    caretLabel.labelType = Label::LABEL_TYPE_NORMAL;
    inputLabel.SetColorEnum(Label::LABEL_COLOR_WHITE);
    caretLabel.SetText("_");
    std::cout << "[Console::GenerateCaretLabel] Successfully generated the caret label.\n";
}

void DebugConsole::GenerateOutputHistory(SDL_State& state)
{
    for (int i = 0; i < outputHistoryMax; i++)
    {
        std::string outputHistoryString;
        outputHistory.push_back(outputHistoryString);
        std::cout << "[DebugConsole::GenerateOutputHistory] Generated: '" << outputHistory.size() << "' items in outputHistory.\n";

    }
}
void DebugConsole::GenerateLabelsForOutput(SDL_State& state)
{
	for (int i = 0; i < outputLines; i++)
	{
		Label outputLabel(state);
        outputLabel.SetFont();
        outputLabel.SetColorRGB(0, 0, 0, 255);
        outputLabel.SetColorEnum(Label::LABEL_COLOR_RED);
        outputLabel.fontSize = fontSize;
        outputLabels.push_back(outputLabel);
	}
}

void DebugConsole::Initialise(SDL_State& state)
{
    GenerateInputLabel(state);
    GenerateCaretLabel(state);
    GenerateOutputHistory(state);
	GenerateLabelsForOutput(state);
    GenerateHistory();
    RegisterCommands();
}

void DebugConsole::GenerateHistory()
{
    historyCount = 0;
    history.clear(); // ensure the history is cleared before readd to the history
    std::ifstream fileInput("console_history.txt");
    if (!fileInput.is_open())
    {
        Output("Error: Failed to open 'console_history.txt'");
        return;
    }
    std::string commands;
    while (fileInput >> commands)
    {
        history.push_back(commands);
        historyCount++;
    }
    std::string line;
    while (std::getline(fileInput, line))
    {
    }
    std::cout << "[DebugConsole::GenerateHistory] historyCount: '" << historyCount << "'\n";
}

void DebugConsole::RenderLabels(SDL_State& state)
{
    // im gonna code the pos for now. remember to change later. make it its own func

    //int test = i - outputIndexOffsetToScroll;
    for (int i = 0; i < outputLines; i++)
    {
        outputLabels[i].Clear();
    }
    for (int i = 0; i < outputLines; i++)
    {
        outputLabels[(outputLines - 1) - i].SetText(outputHistory[i + outputIndexOffsetToScroll]);
    }
    for (int i = 0; i < outputLines; i++)
    {
       
        outputLabels[i].positionX = consoleTextX;
        outputLabels[i].positionY = deleteLater - (offset - i * (fontSize + offset));
        if (active)
        {

            if (outputLabels[i].text.contains("ERROR:") || outputLabels[i].text.contains("Error:"))
            {
                outputLabels[i].SetColorEnum(Label::LABEL_COLOR_RED);
            }
            else if (outputLabels[i].text.contains("WARNING:") || outputLabels[i].text.contains("Warning:"))
            {
                outputLabels[i].SetColorEnum(Label::LABEL_COLOR_YELLOW);
            }
            else
            {
                outputLabels[i].SetColorEnum(Label::LABEL_COLOR_WHITE);
            }
            outputLabels[i].Render();
        }
    }
    //test = i - outputIndexOffsetToScroll;

    //if (test < 0)
    //{
        //test = 0;
        //outputLabels[i].Clear();
        //outputLabels[i].SetText("[" + std::to_string(i) + "] " + "");
    //}
    //else
    //{
    //    
    //}
        
    /*
    outputLabels[0].Clear();
    outputLabels[1].Clear();
    outputLabels[2].Clear();
    outputLabels[3].Clear();
    outputLabels[4].Clear();
    outputLabels[5].Clear();
    outputLabels[6].Clear();
    outputLabels[7].Clear();
    outputLabels[8].Clear();
    outputLabels[9].Clear();
    outputLabels[10].Clear();
    outputLabels[11].Clear();
    outputLabels[12].Clear();
    outputLabels[13].Clear();
    outputLabels[14].Clear();
    outputLabels[15].Clear();
    outputLabels[16].Clear();
    outputLabels[17].Clear();
    outputLabels[18].Clear();
    outputLabels[19].Clear();
    outputLabels[20].Clear();
    outputLabels[21].Clear();
    outputLabels[22].Clear();
    outputLabels[23].Clear();
    outputLabels[24].Clear();
    outputLabels[25].Clear();
    outputLabels[26].Clear();
    outputLabels[27].Clear();
    outputLabels[28].Clear();
    outputLabels[29].Clear();
    outputLabels[30].Clear();
    outputLabels[31].Clear();

    outputLabels[31].SetText(outputHistory[0 + outputIndexOffsetToScroll]);
    outputLabels[30].SetText(outputHistory[1 + outputIndexOffsetToScroll]);
    outputLabels[29].SetText(outputHistory[2 + outputIndexOffsetToScroll]);
    outputLabels[28].SetText(outputHistory[3 + outputIndexOffsetToScroll]);
    outputLabels[27].SetText(outputHistory[4 + outputIndexOffsetToScroll]);
    outputLabels[26].SetText(outputHistory[5 + outputIndexOffsetToScroll]);
    outputLabels[25].SetText(outputHistory[6 + outputIndexOffsetToScroll]);
    outputLabels[24].SetText(outputHistory[7 + outputIndexOffsetToScroll]);
    outputLabels[23].SetText(outputHistory[8 + outputIndexOffsetToScroll]);
    outputLabels[22].SetText(outputHistory[9 + outputIndexOffsetToScroll]);
    outputLabels[21].SetText(outputHistory[10 + outputIndexOffsetToScroll]);
    outputLabels[20].SetText(outputHistory[11 + outputIndexOffsetToScroll]);
    outputLabels[19].SetText(outputHistory[12 + outputIndexOffsetToScroll]);
    outputLabels[18].SetText(outputHistory[13 + outputIndexOffsetToScroll]);
    outputLabels[17].SetText(outputHistory[14 + outputIndexOffsetToScroll]);
    outputLabels[16].SetText(outputHistory[15 + outputIndexOffsetToScroll]);
    outputLabels[15].SetText(outputHistory[16 + outputIndexOffsetToScroll]);
    outputLabels[14].SetText(outputHistory[17 + outputIndexOffsetToScroll]);
    outputLabels[13].SetText(outputHistory[18 + outputIndexOffsetToScroll]);
    outputLabels[12].SetText(outputHistory[19 + outputIndexOffsetToScroll]);
    outputLabels[11].SetText(outputHistory[20 + outputIndexOffsetToScroll]);
    outputLabels[10].SetText(outputHistory[21 + outputIndexOffsetToScroll]);
    outputLabels[9].SetText(outputHistory[22 + outputIndexOffsetToScroll]);
    outputLabels[8].SetText(outputHistory[23 + outputIndexOffsetToScroll]);
    outputLabels[7].SetText(outputHistory[24 + outputIndexOffsetToScroll]);
    outputLabels[6].SetText(outputHistory[25 + outputIndexOffsetToScroll]);
    outputLabels[5].SetText(outputHistory[26 + outputIndexOffsetToScroll]);
    outputLabels[4].SetText(outputHistory[27 + outputIndexOffsetToScroll]);
    outputLabels[3].SetText(outputHistory[28 + outputIndexOffsetToScroll]);
    outputLabels[2].SetText(outputHistory[29 + outputIndexOffsetToScroll]);
    outputLabels[1].SetText(outputHistory[30 + outputIndexOffsetToScroll]);
    outputLabels[0].SetText(outputHistory[31 + outputIndexOffsetToScroll]);

    //outputLabels[i].positionX = consoleTextX;
    outputLabels[0].positionX = consoleTextX;
    outputLabels[1].positionX = consoleTextX;
    outputLabels[2].positionX = consoleTextX;
    outputLabels[3].positionX = consoleTextX;
    outputLabels[4].positionX = consoleTextX;
    outputLabels[5].positionX = consoleTextX;
    outputLabels[6].positionX = consoleTextX;
    outputLabels[7].positionX = consoleTextX;
    outputLabels[8].positionX = consoleTextX;
    outputLabels[9].positionX = consoleTextX;
    outputLabels[10].positionX = consoleTextX;
    outputLabels[11].positionX = consoleTextX;
    outputLabels[12].positionX = consoleTextX;
    outputLabels[13].positionX = consoleTextX;
    outputLabels[14].positionX = consoleTextX;
    outputLabels[15].positionX = consoleTextX;
    outputLabels[16].positionX = consoleTextX;
    outputLabels[17].positionX = consoleTextX;
    outputLabels[18].positionX = consoleTextX;
    outputLabels[19].positionX = consoleTextX;
    outputLabels[20].positionX = consoleTextX;
    outputLabels[21].positionX = consoleTextX;
    outputLabels[22].positionX = consoleTextX;
    outputLabels[23].positionX = consoleTextX;
    outputLabels[24].positionX = consoleTextX;
    outputLabels[25].positionX = consoleTextX;
    outputLabels[26].positionX = consoleTextX;
    outputLabels[27].positionX = consoleTextX;
    outputLabels[28].positionX = consoleTextX;
    outputLabels[29].positionX = consoleTextX;
    outputLabels[30].positionX = consoleTextX;
    outputLabels[31].positionX = consoleTextX;

    //outputLabels[i].positionY = deleteLater - (offset - i * (fontSize + offset)); // this calc adds the padding offset at the starts then the offset padding after/inbetween
    outputLabels[0].positionY = deleteLater - (offset - 0 * (fontSize + offset));
    outputLabels[1].positionY = deleteLater - (offset - 1 * (fontSize + offset));
    outputLabels[2].positionY = deleteLater - (offset - 2 * (fontSize + offset));
    outputLabels[3].positionY = deleteLater - (offset - 3 * (fontSize + offset));
    outputLabels[4].positionY = deleteLater - (offset - 4 * (fontSize + offset));
    outputLabels[5].positionY = deleteLater - (offset - 5 * (fontSize + offset));
    outputLabels[6].positionY = deleteLater - (offset - 6 * (fontSize + offset));
    outputLabels[7].positionY = deleteLater - (offset - 7 * (fontSize + offset));
    outputLabels[8].positionY = deleteLater - (offset - 8 * (fontSize + offset));
    outputLabels[9].positionY = deleteLater - (offset - 9 * (fontSize + offset));
    outputLabels[10].positionY = deleteLater - (offset - 10 * (fontSize + offset));
    outputLabels[11].positionY = deleteLater - (offset - 11 * (fontSize + offset));
    outputLabels[12].positionY = deleteLater - (offset - 12 * (fontSize + offset));
    outputLabels[13].positionY = deleteLater - (offset - 13 * (fontSize + offset));
    outputLabels[14].positionY = deleteLater - (offset - 14 * (fontSize + offset));
    outputLabels[15].positionY = deleteLater - (offset - 15 * (fontSize + offset));
    outputLabels[16].positionY = deleteLater - (offset - 16 * (fontSize + offset));
    outputLabels[17].positionY = deleteLater - (offset - 17 * (fontSize + offset));
    outputLabels[18].positionY = deleteLater - (offset - 18 * (fontSize + offset));
    outputLabels[19].positionY = deleteLater - (offset - 19 * (fontSize + offset));
    outputLabels[20].positionY = deleteLater - (offset - 20 * (fontSize + offset));
    outputLabels[21].positionY = deleteLater - (offset - 21 * (fontSize + offset));
    outputLabels[22].positionY = deleteLater - (offset - 22 * (fontSize + offset));
    outputLabels[23].positionY = deleteLater - (offset - 23 * (fontSize + offset));
    outputLabels[24].positionY = deleteLater - (offset - 24 * (fontSize + offset));
    outputLabels[25].positionY = deleteLater - (offset - 25 * (fontSize + offset));
    outputLabels[26].positionY = deleteLater - (offset - 26 * (fontSize + offset));
    outputLabels[27].positionY = deleteLater - (offset - 27 * (fontSize + offset));
    outputLabels[28].positionY = deleteLater - (offset - 28 * (fontSize + offset));
    outputLabels[29].positionY = deleteLater - (offset - 29 * (fontSize + offset));
    outputLabels[30].positionY = deleteLater - (offset - 30 * (fontSize + offset));
    outputLabels[31].positionY = deleteLater - (offset - 31 * (fontSize + offset));

        
    //}
    if (active)
    {
        outputLabels[0].Render();
        outputLabels[1].Render();
        outputLabels[2].Render();
        outputLabels[3].Render();
        outputLabels[4].Render();
        outputLabels[5].Render();
        outputLabels[6].Render();
        outputLabels[7].Render();
        outputLabels[8].Render();
        outputLabels[9].Render();
        outputLabels[10].Render();
        outputLabels[11].Render();
        outputLabels[12].Render();
        outputLabels[13].Render();
        outputLabels[14].Render();
        outputLabels[15].Render();
        outputLabels[16].Render();
        outputLabels[17].Render();
        outputLabels[18].Render();
        outputLabels[19].Render();
        outputLabels[20].Render();
        outputLabels[21].Render();
        outputLabels[22].Render();
        outputLabels[23].Render();
        outputLabels[24].Render();
        outputLabels[25].Render();
        outputLabels[26].Render();
        outputLabels[27].Render();
        outputLabels[28].Render();
        outputLabels[29].Render();
        outputLabels[30].Render();
        outputLabels[31].Render();
    }
    */
    if (active)
    {
        inputLabel.Clear();
        inputLabel.SetText("] " + inputText);
        inputLabel.Render();

        caretLabel.Clear();
        caretLabel.SetText("_");
        caretLabel.Render();
    }
}

void DebugConsole::Output(std::string output)
{
    //historyIndex++;
    //OutputElement newOutput;
    //newOutput.baseIndex = historyIndex;
    //newOutput.text = output;
    //outputHistory.push_back(output);
    //for (int i = 1; i < historyMax; i++)
    //{
    //    outputHistory[i - 1] = outputHistory[i];
    //}
    //outputHistory[historyMax - 1] = output;

    //for (int i = 1; i < outputHistoryMax; i++)
    //{
    //    outputHistory[i - 1] = outputHistory[i];
    //}

    //outputHistory[outputLines - 1] = output;

    // i have outputNextIndex. i want to have my vector filled with blank strings and i increment a outputNextIndex to indictae the next element to replace.
    for (int i = outputHistoryMax - 1; i > 0; i--)
    {
        // 127 is the max in the vector. we want to get rid of it inplace for everything moving up 1 - thus leaving idex 0 free for the new output.
        outputHistory[i] = outputHistory[i - 1];
    }

    //std::string teststring = "[" + std::to_string(outputNextIndex) + "] " + output;

    outputHistory[0] = output;
    outputNextIndex++;
}

void DebugConsole::OutputError(std::string output)
{
    for (int i = outputHistoryMax - 1; i > 0; i--)
    {
        outputHistory[i] = outputHistory[i - 1];
    }
    std::string newOutput = "Error: " + output;
    outputHistory[0] = newOutput;
    outputNextIndex++;
}

void DebugConsole::OutputWarning(std::string output)
{
    for (int i = outputHistoryMax - 1; i > 0; i--)
    {
        outputHistory[i] = outputHistory[i - 1];
    }
    std::string newOutput = "Warning: " + output;
    outputHistory[0] = newOutput;
    outputNextIndex++;
}

void DebugConsole::ScrollOutput(int direction)
{

    if (direction == 0)
    {
        if (outputLabels.size() >= outputLines)
        {
            if (outputIndexOffsetToScroll == outputHistoryMax - outputLines)
            {
                std::cout << "[DebugConsole::ScrollOutput] outputIndexOffsetToScroll: '" << outputIndexOffsetToScroll << "' Scrolled up but at max so no going higher. \n";
                return;
            }
            else if (outputIndexOffsetToScroll < outputNextIndex - outputLines)
            {
                outputIndexOffsetToScroll += 1;
                scrollMode = true;
                std::cout << "[DebugConsole::ScrollOutput] outputIndexOffsetToScroll: '" << outputIndexOffsetToScroll << "' Scrolled up. \n";
            }
            
        }
    }
    else if (direction == 1)
    {
        if (outputIndexOffsetToScroll == 0)
        {
            return;
        }
        else
        {

        }
        outputIndexOffsetToScroll -= 1;
        scrollMode = true;
        std::cout << "[DebugConsole::ScrollOutput] outputIndexOffsetToScroll: '" << outputIndexOffsetToScroll << "' Scrolled down.\n";
    }

}

void DebugConsole::UpdateInputLabelPosition(SDL_State& state)
{
    inputLabel.positionX = consoleTextX;
    inputLabel.positionY = offset + outputLinesCurrent * (fontSize + offset);
}

void DebugConsole::RemoveCharFromString()
{
    if (position == 0)
    {
        // idk how to fix this other than this safeguard
        return;
    }
    else if (inputText.size() > 0)
    {
        //std::cout << "Position in the string index of the character i want to erase: " << position << "\n";

        position -= 1;
        inputText.erase(inputText.begin() + position);
        std::cout << "[DebugConsole::RemoveCharFromString] '" <<inputText.size() << "'\n";
    }
    
}

void DebugConsole::AddCharToString(int input)
{
    if (position > -1)
    {
        if (position - 1 < inputText.size())
        {
            char a = input;
            //std::cout << "Char i want to add to the list: " << a << "\n";
            inputText.insert(inputText.begin() + position, a);
            MovePosition(1);
            //std::cout << inputText << "\n";

        }
        else if (inputText.size() == 0 || position == 0)
        {
            //std::cout << "inputText.size() is equal to : " << inputText.size() << "\n";
            char a = input;
            //std::cout << "Char i want to add to the list: " << a << "\n";
            inputText.insert(inputText.begin() + position, a);
            MovePosition(1);
            //std::cout << inputText << "\n";

        }
    }
    //std::cout << "Hello!\n";
}

void DebugConsole::MovePosition(int direction)
{
    // i need to clamp somehow before i move position
    // i think this will work. if less than max and atleast than 0.
    if (direction == 1)
    {
        position += 1;
    }
    else if (direction == 0)
    {
        position -= 1;
    }
    else if (direction == 2)
    {
        position = 0;
    }
    else if (direction == 3)
    {
        position = inputText.size();
    }
    //std::cout << "New position in string index is now: " << position << "\n";
    if (position < inputText.size())
    {
        //std::cout << line.size() << "\n";
    }

    if (position < 0)
    {
        position = 0;
        //std::cout << "[WARNING] Position found below 0! Setting to index 0! (" << position << ")\n";
    }

    if (position > inputText.size())
    {
        //std::cout << "[WARNING] Position out of index! (" << position << ")\n";
        //std::cout << "[WARNING] Position found above inputText.size() (aka '" << inputText.size() << "') Setting to index inputText.size()! (" << inputText.size() << ")\n";
        position = inputText.size();
    }
}

void DebugConsole::RenderBackground(SDL_State& state)
{
    if (active)
    {
        SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(state.renderer, backgroundColorR, backgroundColorG, backgroundColorB, backgroundColorA);
        SDL_RenderFillRect(state.renderer, &backgroundTransform);
        if (!SDL_RenderFillRect(state.renderer, &backgroundTransform))
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create texture.", nullptr);
        }
    }
}

void DebugConsole::GrabWindowSizeForConsole(SDL_State& state)
{
    backgroundSizeWidth = state.width;
    backgroundSizeHeight = offset + outputLinesCurrent * (inputLabel.fontSize + offset) + inputLabel.fontSize + offset; // include +8 because we also want to cover the input label in the background. offset for the input padding/
    backgroundTransform =
    {
        backgroundPositionX,
        backgroundPositionY,
        backgroundSizeWidth,
        backgroundSizeHeight, 
    };
}

void DebugConsole::CaretLabelAnimate(double deltaTime)
{
    caretFlashTimer += 1.0f * (float)deltaTime;
    if (caretFlashTimer > 1.0f)
    {
        caretFlashTimer = 0.0f;
    }

    if (caretFlashTimer > 0.5f)
    {
        caretLabel.SetColorRGB(255, 255, 255, 255);
    }
    else if ((caretFlashTimer < 0.5f))
    {
        caretLabel.SetColorRGB(255, 255, 255, 0);
    }
    //std::cout << "[DebugConsole::CaretLabelAnimate] caretFlashTimer = '" << caretFlashTimer << "'\n";
}

void DebugConsole::CaretLabelPosition()
{
    caretLabel.positionX = consoleTextX + (position * fontSize) + (fontSize * 2); // looks like a mess but trust.
    caretLabel.positionY = offset + outputLinesCurrent * (fontSize + offset);
}

void DebugConsole::ProcessCommand(SDL_State& state)
{
    /*
    int calculatedNumb = outputLines - 1; // would be 32 but - 1 is 31
    for (int i = 0; i < outputLines; i++)
    {
        outputHistory[calculatedNumb] = inputText;
        outputHistory[calculatedNumb - i] = outputHistory[calculatedNumb]; // set 30 to be what 31 was.
        outputHistory[calculatedNumb + i] = 
    }
    */
    
    //outputHistory[30] = outputHistory[31]; // set the old to the new.

    //the formula is look at in front, grab it and send it to before.

    // reset the input
    // print the current entered input. indiciated with the ] 
    Output("] " + inputText);
    validCommand = true;
    ExecuteCommand(inputText);
    
    
}

void DebugConsole::ScrollHistory(int direction)
{
    if (active)
    {
        if (direction == 0)
        {
            if (historyIndex == 0)
            {
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is == 0.\n";
                historyIndex = 0;
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is currently " << historyIndex << " so the text should be '" << history[historyIndex] << "'\n";

                for (int i = 0; i < history.size(); i++)
                {
                    //std::cout << i << ": '" << history[i] << "'\n";
                }
                return;
            }
            else
            {
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is > 0.\n";
                historyIndex -= 1;
                inputText = history[historyIndex];
                MovePosition(3); // 3 sets the position to the end of the text.
                //MovePosition(2); // 2 sets the position to 0
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is currently " << historyIndex << " so the text should be '" << history[historyIndex] << "'\n";

                for (int i = 0; i < history.size(); i++)
                {
                    //std::cout << i << ": '" << history[i] << "'\n";
                }
            }
        }
        if (direction == 1)
        {
            if (historyIndex >= history.size())
            {
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is >= history.size().\n";
                historyIndex = history.size();
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is currently " << historyIndex << " so the text should be '" << history[historyIndex] << "'\n";

                for (int i = 0; i < history.size(); i++)
                {
                    //std::cout << i << ": '" << history[i] << "'\n";
                }
                return;
            }
            else
            {
                if (historyIndex == history.size() - 1)
                {
                    //std::cout << "[DebugConsole::ScrollHistory] DO NOT MOVE ANYMORE UPWARD.\n";
                    return;
                }
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is < history.size()\n";
                if (history.size() > 0)
                {
                    historyIndex += 1;
                }
                //std::cout << "[DebugConsole::ScrollHistory] Setting inputText = history[historyIndex]. (Note: history.size() == " << history.size() << " and historyIndex == " << historyIndex << ".\n";
                if (historyIndex == history.size())
                {
                    //std::cout << "[DebugConsole::ScrollHistory] historyIndex is historyIndex == history.size() - 1 so skip.\n";
                    historyIndex = history.size();
                    return;
                }
                inputText = history[historyIndex];
                MovePosition(3); // 3 sets the position to the end of the text.
                //std::cout << "[DebugConsole::ScrollHistory] historyIndex is currently " << historyIndex << " so the text should be '" << history[historyIndex] << "'\n";
                for (int i = 0; i < history.size(); i++)
                {
                    //std::cout << i << ": '" << history[i] << "'\n";
                }
            }
        }
    }
}

void DebugConsole::AutoComplete()
{
    // for each char , check each command's name if those char's match the command's name char's.
    /*for (int i = 0; i < inputText.size(); i++)
    {
        for (int j = 0; j < commands.size(); j++)
        {
            if (inputText[i] == commands[j].commandName[i])
            {
                std::cout << "[DebugConsole::AutoComplete()] '" << inputText[i] << "' is the same as '" << commands[j].commandName[i] << "'?\n";
            }
            else
            {
                std::cout << "[DebugConsole::AutoComplete()] Uh oh, Something went wrong!\n";
            }
        }
    }
    */
    // if char[] matchess i && including others.
    int matches = 0;
    int matchingCharactersInTheEnd = 0;
    std::string matchingCharacters = "";
    bool savedMatch = false;
    int copies = 0;
    std::vector<Command> matchingCommands;
    for (int j = 0; j < commands.size(); j++)
    {
        if (commands[j].commandName.contains(inputText))
        {
            if (commands[j].commandName[0] == inputText[0])
            {
                /*
                for (int i = 0; i < inputText.size(); i++)
                {
                    if (!savedMatch)
                    {
                        matchingCharacters += commands[j].commandName[i];
                    }
                    if (i == inputText.size())
                    {
                        matchingCharacters.clear();
                    }
                    
                }
                */
                matches++;
                matchingCommands.push_back(commands[j]);

                std::cout << "[DebugConsole::AutoComplete()] Index: '" << matchingCommands.size() << "' " << commands[j].commandName << "' contains '" << inputText << "'!\n";
            }
            
        }
        
    }
    
    if (matches == 0)
    {
        Output("No console commands found to match '" + inputText + "'");
    }
    else if (matches == 1)
    {
        inputText = matchingCommands[0].commandName;
        MovePosition(3);
    }
    else if (matches > 1)
    {
        /*
        // here i need to compare the matches and find where the matches stop.
        for (int i = 0; i < matchingCommands.size(); i++)
        {
            for (int j = 0; j < matchingCommands[i].commandName[j]; j++)
            {
                std::cout << "[DebugConsole::AutoComplete()] Command name '" << matchingCommands[i].commandName << ", '" << matchingCommands[i].commandName[j] << "'\n";
                // omg imagine if this idea works.
                // a char vector. adds all matching commands chars in it.
                // we also have a vector int of the command names size.
                // we check from the start of every command name size, every character in the matching command chars.
            }
        }
        */
        int index = 0;
        char currentCharacter = 0;
        char comparingCharacter = 0;
        int failedMatch = 0;
        bool done = false;

        char buildStringChar = 0;
        std::string buildStringString = "";

        // start with something.
        currentCharacter = matchingCommands[0].commandName[0];
        comparingCharacter = currentCharacter;

        for (int i = 0; i < matchingCommands.size(); i++)
        {
            std::cout << "[DebugConsole::AutoComplete()] --- currentCharacter: '" << currentCharacter << "'\n";
            std::cout << "[DebugConsole::AutoComplete()] --- comparingCharacter: '" << comparingCharacter << "'\n";
            std::cout << "[DebugConsole::AutoComplete()] --- i: '" << i << "'\n";
            std::cout << "[DebugConsole::AutoComplete()] --- matchingCommands.size(): '" << matchingCommands.size() << "'\n";
            currentCharacter = matchingCommands[i].commandName[index];
            if (currentCharacter != comparingCharacter)
            {
                // we found where the match stops
                failedMatch = index;
                done = true;
                std::cout << "[DebugConsole::AutoComplete()] --- The current string: '" << buildStringString << "' ended with the non-matching character: '" << currentCharacter << "' when it should be: '" << comparingCharacter << "'\n";
                break;
            }
            if (!done && i + 1 == matchingCommands.size()) // we can confirm, its a safe match so increment building the sstring
            {
                std::cout << "[DebugConsole::AutoComplete()] --- i: '" << i << "' which is the same as matchingCommands.size(): '"<< matchingCommands.size() << "'\n";
                i = 0; // i think this sets to 0
                index++;
                buildStringChar = comparingCharacter;
                buildStringString += buildStringChar;
                comparingCharacter = matchingCommands[i].commandName[index];
                std::cout << "[DebugConsole::AutoComplete()] --- Reached the end of the command: '" << buildStringString << "'\n";
            }
            std::cout << "[DebugConsole::AutoComplete()] --- The current string: '" << buildStringString << "'\n";
        }

        inputText = buildStringString;
        MovePosition(3);

        Output("Possible commands for prefix '" + inputText + "'");
        for (int i = 0; i < matchingCommands.size(); i++)
        {
            Output(matchingCommands[i].commandName + " - " + matchingCommands[i].commandDescription);
        }
    }
    


    for (int i = 0; i < matchingCommands.size(); i++)
    {
        std::cout << "[DebugConsole::AutoComplete()] Element '" << i << "' in 'matchingCommands' = '" << matchingCommands[i].commandName << "'\n";
    }
    // cleanup
    matches = 0;
    matchingCommands.clear();
}

void DebugConsole::Update(SDL_State& state, double deltaTime)
{
    GrabWindowSizeForConsole(state);
    RenderBackground(state);
    RenderLabels(state);
    AnimateConsole(state, deltaTime);
    CaretLabelPosition();
    CaretLabelAnimate(deltaTime);
}

void DebugConsole::GrabInputEvents(SDL_Event& event)
{
    if (active)
    {
        // my totally efficient ascii input system
        if (strcmp(event.text.text, " ") == 0)
        {
            char input = 32;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "!") == 0)
        {
            char input = 33;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, R"(")") == 0)
        {
            char input = 34;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "#") == 0)
        {
            char input = 35;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "$") == 0)
        {
            char input = 36;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "&") == 0)
        {
            char input = 37;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "&") == 0)
        {
            char input = 38;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "'") == 0)
        {
            char input = 39;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "(") == 0)
        {
            char input = 40;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, ")") == 0)
        {
            char input = 41;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "*") == 0)
        {
            char input = 42;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "+") == 0)
        {
            char input = 43;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, ",") == 0)
        {
            char input = 44;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "-") == 0)
        {
            char input = 45;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, ".") == 0)
        {
            char input = 46;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "/") == 0)
        {
            char input = 47;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "0") == 0)
        {
            char input = 48;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "1") == 0)
        {
            char input = 49;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "2") == 0)
        {
            char input = 50;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "3") == 0)
        {
            char input = 51;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "4") == 0)
        {
            char input = 52;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "5") == 0)
        {
            char input = 53;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "6") == 0)
        {
            char input = 54;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "7") == 0)
        {
            char input = 55;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "8") == 0)
        {
            char input = 56;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "9") == 0)
        {
            char input = 57;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, ":") == 0)
        {
            char input = 58;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, ";") == 0)
        {
            char input = 59;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "<") == 0)
        {
            char input = 60;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "=") == 0)
        {
            char input = 61;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, ">") == 0)
        {
            char input = 62;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "?") == 0)
        {
            char input = 63;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "@") == 0)
        {
            char input = 64;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "A") == 0)
        {
            char input = 65;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "B") == 0)
        {
            char input = 66;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "C") == 0)
        {
            char input = 67;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "D") == 0)
        {
            char input = 68;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "E") == 0)
        {
            char input = 69;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "F") == 0)
        {
            char input = 70;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "G") == 0)
        {
            char input = 71;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "H") == 0)
        {
            char input = 72;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "I") == 0)
        {
            char input = 73;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "J") == 0)
        {
            char input = 74;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "K") == 0)
        {
            char input = 75;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "L") == 0)
        {
            char input = 76;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "M") == 0)
        {
            char input = 77;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "N") == 0)
        {
            char input = 78;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "O") == 0)
        {
            char input = 79;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "P") == 0)
        {
            char input = 80;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "Q") == 0)
        {
            char input = 81;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "R") == 0)
        {
            char input = 82;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "S") == 0)
        {
            char input = 83;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "T") == 0)
        {
            char input = 84;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "U") == 0)
        {
            char input = 85;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "V") == 0)
        {
            char input = 86;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "W") == 0)
        {
            char input = 87;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "X") == 0)
        {
            char input = 88;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "Y") == 0)
        {
            char input = 89;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "Z") == 0)
        {
            char input = 90;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "[") == 0)
        {
            char input = 91;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, R"(\)") == 0)
        {
            char input = 92;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "]") == 0)
        {
            char input = 93;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "^") == 0)
        {
            char input = 94;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "_") == 0)
        {
            char input = 95;
            AddCharToString(input);
        }
        /*
        * 
        * this key is for toggling console so we dont want to parse it to the string.
        *  
        if (strcmp(event.text.text, "`") == 0)
        {
            char input = 96;
            AddCharToString(input);
        }
        */
        if (strcmp(event.text.text, "a") == 0)
        {
            char input = 97;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "b") == 0)
        {
            char input = 98;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "c") == 0)
        {
            char input = 99;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "d") == 0)
        {
            char input = 100;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "e") == 0)
        {
            char input = 101;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "f") == 0)
        {
            char input = 102;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "g") == 0)
        {
            char input = 103;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "h") == 0)
        {
            char input = 104;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "i") == 0)
        {
            char input = 105;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "j") == 0)
        {
            char input = 106;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "k") == 0)
        {
            char input = 107;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "l") == 0)
        {
            char input = 108;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "m") == 0)
        {
            char input = 109;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "n") == 0)
        {
            char input = 110;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "o") == 0)
        {
            char input = 111;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "p") == 0)
        {
            char input = 112;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "q") == 0)
        {
            char input = 113;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "r") == 0)
        {
            char input = 114;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "s") == 0)
        {
            char input = 115;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "t") == 0)
        {
            char input = 116;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "u") == 0)
        {
            char input = 117;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "v") == 0)
        {
            char input = 118;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "w") == 0)
        {
            char input = 119;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "x") == 0)
        {
            char input = 120;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "y") == 0)
        {
            char input = 121;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "z") == 0)
        {
            char input = 122;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "{") == 0)
        {
            char input = 123;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "|") == 0)
        {
            char input = 124;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "}") == 0)
        {
            char input = 125;
            AddCharToString(input);
        }
        if (strcmp(event.text.text, "~") == 0)
        {
            char input = 126;
            AddCharToString(input);
        }
    }
}



void DebugConsole::RegisterCommands()
{
    RegisterCommand("help", "Lists all available commands", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        SearchAllConsoleCommands();
        std::cout << "[DebugConsole::RegisterCommands()] SearchAllConsoleCommands()\n";

        return {};
    });

    RegisterCommand("clear", "Clears the console output", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        outputNextIndex = 0;
        std::cout << "[DebugConsole::RegisterCommands()] outputNextIndex = '" << outputNextIndex << "'\n";
        outputIndexOffsetToScroll = 0;
        std::cout << "[DebugConsole::RegisterCommands()] outputIndexOffsetToScroll = '" << outputIndexOffsetToScroll << "'\n";
        for (int i = 0; i < outputHistory.size(); i++)
        {
            outputHistory[i].clear();
            std::cout << "[DebugConsole::RegisterCommands()] outputHistory["<< i << "].clear = '" << outputHistory[i] << "'\n";
        }
        return {};
    });

    RegisterCommand("console_offset_x", "Sets the console offset x", 1, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        consoleTextX = std::stoi(arguments[1]);
        return {};
    }); 

    RegisterCommand("console_set_bg_color", "Set the background color for the console", 4, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        // rgba
        backgroundColorR = std::stoi(arguments[1]);
        backgroundColorG = std::stoi(arguments[2]);
        backgroundColorB = std::stoi(arguments[3]);
        backgroundColorA = std::stoi(arguments[4]);
        return {};
    });

    RegisterCommand("print_to_file", "Print out console commands to the text file console_commands.txt", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        std::ofstream fileOutput("console_commands.txt");
        for (int i = 0; i < commands.size(); i++)
        {
            std::string printCommands = commands[i].commandName + " - " + commands[i].commandDescription + "\n";
            fileOutput << printCommands;
        }
        fileOutput.close();
        return {};
    });

    RegisterCommand("history", "Prints the current history", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        std::ifstream infile("console_history.txt");
        if (!infile.is_open())
        {
            OutputError("Failed to open 'console_history_txt'. Missing file?");
            return{};
        }
        std::string line;
        while (std::getline(infile, line))
        {
            //std::string line2 = ""
            Output(line);
        }
        return {};
    });

    RegisterCommand("clear_history", "Clears the current history", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        history.clear();
        std::ofstream fileOutput("console_history.txt");
        std::string command = "";
        fileOutput << command;
        fileOutput.close();
        return {};
    });

    RegisterCommand("print_output_to_cmd", "prints output to cmd", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        for (int i = 0; i < outputHistory.size(); i++)
        {
            std::cout << "[DebugConsole::RegisterCommands]\nIndex: '" << i << "'\nText: '" << outputHistory[i] << "'\n";
        }
        return {};
    });
    RegisterCommand("print_output_size", "prints output size", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        Output(std::to_string(outputHistory.size()));
        return {};
    });

    RegisterCommand("console_output_error", "Outputs a test error", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        OutputError("This is a test error. Do not worry, nothing is going wrong.");
        return {};
    });

    RegisterCommand("console_output_warning", "Outputs a test warning", 0, [this](const std::vector<std::string>& arguments) -> std::vector<std::string>
    {
        OutputWarning("This is a test warning. Do not worry, nothing is going wrong.");
        return {};
    });
}
