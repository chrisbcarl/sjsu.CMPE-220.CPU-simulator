# sjsu.CMPE-220.CPU-simulator
CPU simulator for SJSU 2026F CMPE 220 System Software instructed by Ishie Eswar.

# About
This is a simulated CPU, emulating a processor that executes binary code compiled from simple C programs.

## CPU Architecture Features
- 32-bit RISC Architecture
    - 32-bit word size
    - R-type, I-type, J-type instructions
    - data-path is unified and shared among all instructions
- Single Cycle
- L1 RAM

## Simulator Features
- input: .bin files with 32-bit words
- output: text log with the memory, register, and telemetry information

## Project Features
- documentation
- collaborative group effort: no slackers
- no AI usage in academically relevant files or labor (see [ai-disclaimer](#ai-disclaimer))
- unionized (kidding)


# Development Setup
```bash
git clone https://github.com/chrisbcarl/sjsu.CMPE-220.CPU-simulator
cd sjsu.CMPE-220.CPU-simulator
code ./  # launch vscode

# setup custom git user with SJSU email address
git config user.name "First Last"
git config user.email "first.last@sjsu.com"

# setup python virtualenv, requires 3.10+
poetry config virtualenvs.in-project true --local
poetry install
```


# Run
- Windows
```powershell
# dependencies - gnu C for win32
# RUN AS ADMINISTRATOR
Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope LocalMachine  # WARNING: run any .ps1 script, dangerous but as a developer you are an adult
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
# restart the terminal
choco install mingw -y

# restart the terminal (non-admin)
# build n run
.\build.ps1
```

- Linux
```bash
# dependencies - gnu C + make
# RUN AS ADMINISTRATOR
sudo apt update -y
sudo apt upgrade -y && sudo shutdown -r now
sudo apt install build-essential -y

# build n run
make -f ./Makefile
```

# Authors
|Name               |Email                         |Github Username           |Contributions                                        |
|---                |---                           |---                       |---                                                  |
|Chris Carl         |<chris.carl@sjsu.edu>         |chrisbcarl, chriscarl-sjsu|Project management, dev ops, code architecture design|
|Harsha Krishnaswamy|<harsha.krishnaswamy@sjsu.edu>|harshakrishna15           |                                                     |
|Donovan Burk       |<donovan.burk@sjsu.edu>       |ReverseBack2              |                                                     |
|Jehu Zepeda-Silva  |<jehu.zepeda-silva@sjsu.edu>  |jehuzepeda-silva-rgb      |                                                     |
|Mark Haiko         |<mark.haiko@sjsu.edu>         |Bageldorf                 |                                                     |



## AI Disclaimer
All AI use is declared to be ethical and deliberate. We do not and will not use AI to affect academically related work. It will only be used in the cases where its usage does not contribute to the understanding of the concepts of Computer Architecture and Organization. All usage instances will include the product model description, prompt, datetime or URL for tracability.

Unless specified, all disclaimed items were **modified** after content and not included in this project verbatim.

The following items are declared to have been assisted by AI:
- Makefile (Chris Carl)

