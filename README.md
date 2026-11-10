# sjsu.CMPE-220.CPU-simulator
CPU simulator for SJSU 2026F CMPE 220 System Software instructed by Ishie Eswar. It simulates a simple, custom processor that executes binary code compiled from C programs.

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

wsl make  # alternative
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
|Harsha Krishnaswamy|<harsha.krishnaswamy@sjsu.edu>|harshakrishna15           |Example Programs                                     |
|Donovan Burk       |<donovan.burk@sjsu.edu>       |ReverseBack2              |Core Diagram/Structs                                 |
|Jehu Zepeda-Silva  |<jehu.zepeda-silva@sjsu.edu>  |jehuzepeda-silva-rgb      |ISA Definition                                       |
|Mark Haiko         |<mark.haiko@sjsu.edu>         |Bageldorf                 |Example Programs                                     |
|Sriharsha Edukulla |<sriharsha.edukulla@sjsu.edu> |sriharsha8000             |                                                     |



## AI Disclaimer
The following files are declared to have been partially developed with the use of generative AI:
- Chris Carl
    - `Makefile`
- Jehu Zepeda-Silva
    - `isa.md`, `isa.h`, `isa.c`

