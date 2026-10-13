# sjsu.CMPE-220.CPU-simulator
CPU simulator for SJSU 2026F CMPE 220 System Software instructed by Ishie Eswar.


# Run
- Windows
```powershell
# administrator
Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope LocalMachine  # WARNING: run any .ps1 script, dangerous but as a developer you are an adult
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
# restart the terminal
choco install mingw -y


```

- Linux
```bash
# dependencies
sudo apt update -y
sudo apt upgrade -y && sudo shutdown -r now
sudo apt install build-essential -y

# build n run
make -f ./Makefile
```


# Development Setup
```powershell
# setup custom git user with sjsu email address
git config user.name "First Last"
git config user.email "first.last@sjsu.com"

# setup python virtualenv, requires 3.10+
poetry config virtualenvs.in-project true --local
poetry install
```