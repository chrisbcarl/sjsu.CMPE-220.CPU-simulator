# sjsu.CMPE-220.CPU-simulator
CPU simulator for SJSU 2026F CMPE 220 System Software instructed by Ishie Eswar.


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
- Chris Carl <chris.carl@sjsu.edu>
- Harsha Krishnaswamy <harsha.krishnaswamy@sjsu.edu>
- Donovan Burk <donovan.burk@sjsu.edu>
- Jehu Zepeda-Silva <jehu.zepeda-silva@sjsu.edu>
- Mark Haiko <mark.haiko@sjsu.edu>
