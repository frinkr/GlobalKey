# Global Key

A cross platform tool to run command with global hotkey binding.

## Build

**Prerequisites**

* cmake, python (optinal)
* C++ toolchain which is C++17 and up.

**Get the Repo:**

```bash
git clone https://github.com/frinkr/GlobalKey.git
```

**CMake gen:**

```bash
cd GlobalKey
python cmake-gen.py
```

Checkout the build directory to find the project files.



# Usage

The GlobalKey runs as a deamon, and have a tray item (Windows), or a status bar item (MacOS). It loads the config file, run the command when a registered hotkey is triggered. 



##### Config file path

- Mac: `/Users/<name>/Library/Application Support/GlobalKey/GlobalKey.json`
- Windows: `C:\ProgramData\GlobalKey\GlobalKey.json`



**Sample config**

```json
{
    "F1": "toggle org.gnu.Emacs",
    "F2": "toggle /Applications/iTerm.app",
    "F3": "toggle /Applications/Google Chrome.app",
    "F4": "toggle com.apple.dt.Xcode",
    "SHIFT+PAGEDOWN": "volume -1",
    "SHIFT+PAGEUP": "volume +1"
}
```

