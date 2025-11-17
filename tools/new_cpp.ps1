<#
.SYNOPSIS
  Create a new C++ source file from a boilerplate template.

.DESCRIPTION
  Creates a .cpp file with common includes, typedefs and a ready-to-edit main()
  function. Ensures the target directory exists. Optionally opens the created
  file in Zed if `zed` is available on PATH or when -Open is supplied.

USAGE
  .\new_cpp.ps1 <path/to/file.cpp>
  .\new_cpp.ps1 -FilePath src\examples\demo.cpp -Open

PARAMETERS
  -FilePath <string>
    Relative or absolute path to the file to create. If omitted, you'll be
    prompted to enter a filename.

  -Force
    Overwrite the file if it already exists (no prompt).

  -Open
    Attempt to open the created file in the `zed` editor (if available).

.EXAMPLE
  .\new_cpp.ps1 MySolution.cpp
#>

param(
    [Parameter(Mandatory=$false, Position=0)]
    [string]$FilePath,

    [switch]$Force,

    [switch]$Open
)

function Show-Usage {
    Write-Host "Usage: new_cpp.ps1 <relative-or-absolute-path-to-file.cpp> [-Force] [-Open]"
    Write-Host ""
    Write-Host "If you omit the path, you'll be prompted to enter a filename."
}

# Prompt for filename if not provided
if (-not $FilePath -or $FilePath.Trim() -eq "") {
    $FilePath = Read-Host "Enter new .cpp filename (relative to current dir)"
}

if (-not $FilePath) {
    Show-Usage
    exit 1
}

# Ensure extension
$ext = [System.IO.Path]::GetExtension($FilePath)
if ([string]::IsNullOrEmpty($ext)) {
    $FilePath = "$FilePath.cpp"
} elseif ($ext -ne ".cpp") {
    Write-Host "Note: file extension is '$ext' (not '.cpp'). Proceeding with the supplied name."
}

# Resolve full path
try {
    $resolved = Resolve-Path -LiteralPath $FilePath -ErrorAction Stop
    $fullPath = $resolved.ProviderPath
} catch {
    # If it doesn't exist yet, combine with current dir
    $fullPath = Join-Path -Path (Get-Location) -ChildPath $FilePath
    # Normalize
    $fullPath = [System.IO.Path]::GetFullPath($fullPath)
}

$dir = [System.IO.Path]::GetDirectoryName($fullPath)
if (-not [string]::IsNullOrEmpty($dir) -and -not (Test-Path $dir)) {
    try {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "Created directory: $dir"
    } catch {
        Write-Host "Error: Could not create directory: $dir"
        exit 2
    }
}

if (Test-Path $fullPath) {
    if ($Force) {
        # proceed to overwrite
        Write-Host "Overwriting existing file: $fullPath"
    } else {
        $yn = Read-Host "File already exists: $fullPath`nDo you want to overwrite it? (y/N)"
        if ($yn -notmatch '^[yY](es)?$') {
            Write-Host "Aborted. File not created."
            exit 3
        }
    }
}

$filename = [System.IO.Path]::GetFileName($fullPath)
$stem = [System.IO.Path]::GetFileNameWithoutExtension($fullPath)
$date = Get-Date -Format "yyyy-MM-dd"

# Boilerplate content
$boilerplate = @"
// File: $filename
// Created: $date
// Author:
// Description:

#include <bits/stdc++.h>
using namespace std;

// Short type aliases
using ll = long long;
using ull = unsigned long long;
using vi = vector<int>;
using vll = vector<long long>;

// Constants
// constexpr int INF = 1e9;

// Split into helper functions here
// e.g. string read_line() { ... }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Example:
    // int n;
    // if (!(cin >> n)) return 0;
    // vector<int> a(n);
    // for (int &x : a) cin >> x;

    // TODO: implement

    return 0;
}
"@

# Write file (UTF8 without BOM)
try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($boilerplate)
    [System.IO.File]::WriteAllBytes($fullPath, $bytes)
    Write-Host "Created: $fullPath"
} catch {
    Write-Host "Error: failed to write file: $_"
    exit 4
}

# Optionally open in Zed if available or when -Open is set
if ($Open) {
    $zedCmd = Get-Command -Name zed -ErrorAction SilentlyContinue
    if ($zedCmd) {
        try {
            Start-Process -FilePath "zed" -ArgumentList "`"$fullPath`"" -WorkingDirectory $dir
            Write-Host "Opening in Zed..."
        } catch {
            Write-Host "Warning: failed to start Zed: $_"
        }
    } else {
        Write-Host "Note: 'zed' not found on PATH. Install Zed or add it to PATH to use -Open."
    }
}
