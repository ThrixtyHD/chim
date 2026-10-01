# chim
This is a WIP SDL3 project I developed to test my knowledge of C++. It features a debugging console aimed to speed up testing certain features.

### Functionality

- **Autocomplete** - After checking all commands that match the current prefix, it checks each command's character and it's position to figure out the closest prefix.

### Controls
You have your standard `Enter` and `Backspace` submitting and removing characters as well as `Left` and `Right` moving the position.
| Key | Action |
|-----|--------|
| `` ` `` | Open/Close console |
| `Tab` | Autocomplete |
| `Up` | Scroll history up |
| `Down` | Scroll history down |
| `Page Up` | Scroll output up |
| `Page Down` | Scroll output up |

### Setup
The console and label requires linking with the SDL3 window and renderer. I set up my inputs like this: ```if (keys[SDL_SCANCODE_BACKSPACE])
{
    console.RemoveCharFromString();
}
if (keys[SDL_SCANCODE_UP])
{
    console.ScrollHistory(1);
}
if (keys[SDL_SCANCODE_DOWN])
{
    console.ScrollHistory(0);
}
if (keys[SDL_SCANCODE_LEFT])
{
    console.MovePosition(0);
}
if (keys[SDL_SCANCODE_RIGHT])
{
    console.MovePosition(1);
}
if (keys[SDL_SCANCODE_RETURN])
{
    console.ProcessCommand(state);
}
if (keys[SDL_SCANCODE_TAB])
{
    console.AutoComplete();
}
if (keys[SDL_SCANCODE_PAGEUP])
{
    console.ScrollOutput(0);
}
if (keys[SDL_SCANCODE_PAGEDOWN])
{
    console.ScrollOutput(1);
}```
The label system works with a monospaced ASCII char sheet: ``` !"#$%&'()*+,-./0123456789:;<=>?​@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_​`abcdefghijklmnopqrstuvwxyz{|}~```.
In the repo, I have supplied my debug font. It is 8x8px.
