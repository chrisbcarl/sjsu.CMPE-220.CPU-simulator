# GitHub
Doc about GitHub, tips, tricks, techniques, etc.


# Automated Issue Creation
```powershell
choco install gh -y
gh auth login  # skip ssh option
gh auth refresh -h github.com -s admin:public_key
gh ssh-key add --title "envy-fc0033dx" ~/.ssh/id_ed25519.pub
gh extension install ntsk/gh-issue-bulk-create
```

```log
? Where do you use GitHub? GitHub.com
! Did you know that expiring tokens can be requested with --expiring? Read more at https://gh.io/gh-expiring-tokens
? How would you like to authenticate GitHub CLI? Login with a web browser
? What is your preferred protocol for Git operations on this host? SSH
? Upload your SSH public key to your GitHub account? Skip

! One-time code (4309-5881) copied to clipboard
Press Enter to open https://github.com/login/device in your browser...
✓ Authentication complete.
- gh config set -h github.com git_protocol ssh
✓ Configured git protocol
✓ Logged in as chrisbcarl
PS C:\Users\chris\src\sjsu.CMPE-220.CPU-simulator>
```

```powershell
gh issue-bulk-create `
    --template "./.github/artifacts/issue-template.yaml" `
    --csv "./.github/artifacts/issues-m1.csv" `
    --repo "chrisbcarl/sjsu.CMPE-220.CPU-simulator" `
    --dry-run
```

```yaml
---
title: "{{title}}"
labels: "{{labels}}"
assignees: "{{assignees}}"
milestone: ["{{milestone}}"]
---
## Description
{{description}}

## Reproduce
{{steps}}
```

```csv
milestone,title,labels,assignees,description,steps
m1,bug: fix running on ubuntu,bug,chrisbcarl,as it says on the tin,"comma, separated, list"
```

# Labels
- bug
- docs
- duplicate
- feature

# Authors
- chrisbcarl, chriscarl-sjsu
- harshakrishna15
- ReverseBack2
- jehuzepeda-silva-rgb
- Bageldorf



# Milestones
|milestone|description         |due date         |
|---      |---                 |---              |
|m1       |plans and prototypes|November 21, 2026|
|m2       |simulator done      |December 5, 2026 |
|r1       |release             |December 19, 2026|
