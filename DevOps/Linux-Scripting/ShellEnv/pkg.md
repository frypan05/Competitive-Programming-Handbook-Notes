# Linux Package Management - Complete Notes with Examples

## 1. Software Distribution Overview

**Definition**: Software packages are collections of files compiled into single units, managed by package managers.

### Package Types

- **Debian packages (.deb)**: Used by Debian, Ubuntu, Linux Mint, etc.
- **Red Hat packages (.rpm)**: Used by RHEL, Fedora, CentOS, etc.

### Package Ecosystem

- **Upstream providers**: Original software developers who create and compile the code
- **Package maintainers**: Review, manage, and distribute software as packages
- **End users**: Install packages through package managers

**Example Package Names:**

```bash
# Debian package example
google-chrome-stable_current_amd64.deb

# RPM package example
firefox-91.0-1.fc34.x86_64.rpm
```

## 2. Package Repositories

**Definition**: Central storage locations for packages, typically hosted on the internet.

### Repository Structure Example

```txt
WackyWidgets Repository: http://download.widgets/linux/deb/
├── CoolWidget.deb
├── SuperWidget.deb
├── UltraWidget.deb
└── MegaWidget.deb
```

### Repository Configuration

**Debian systems** - Sources file location:

```bash
$ cat /etc/apt/sources.list
# Main Ubuntu repositories
deb http://archive.ubuntu.com/ubuntu/ focal main restricted
deb http://archive.ubuntu.com/ubuntu/ focal-updates main restricted
deb http://archive.ubuntu.com/ubuntu/ focal universe
deb http://archive.ubuntu.com/ubuntu/ focal multiverse

# Security updates
deb http://security.ubuntu.com/ubuntu/ focal-security main restricted
```

**Adding custom repositories:**

```bash
# Add a new repository source
$ echo "deb http://download.widgets/linux/deb/ /" | sudo tee -a /etc/apt/sources.list

# Update package list after adding repository
$ sudo apt update
```

## 3. File Archiving and Compression

### gzip Compression

**Compress a file:**

```bash
$ gzip mycoolfile
# Creates: mycoolfile.gz (original file is replaced)

$ ls
mycoolfile.gz
```

**Decompress a file:**

```bash
$ gunzip mycoolfile.gz
# Restores: mycoolfile (compressed file is removed)

$ ls
mycoolfile
```

**Keep original file while compressing:**

```bash
$ gzip -k mycoolfile
# Creates: mycoolfile.gz (keeps original mycoolfile)

$ ls
mycoolfile  mycoolfile.gz
```

### tar Archiving

**Create archive:**

```bash
$ tar cvf mytarfile.tar mycoolfile1 mycoolfile2 mycoolfile3
# Creates: mytarfile.tar containing all specified files

$ ls
mytarfile.tar  mycoolfile1  mycoolfile2  mycoolfile3
```

**Archive flags explanation:**

- `c`: Create archive
- `v`: Verbose output (show progress)
- `f`: Specify filename (must be last flag)

**Extract archive:**

```bash
$ tar xvf mytarfile.tar
# Extracts all files from mytarfile.tar

$ ls
mytarfile.tar  mycoolfile1  mycoolfile2  mycoolfile3
```

**Extract flags explanation:**

- `x`: Extract files
- `v`: Verbose output
- `f`: Specify filename

### Combined tar and gzip Operations

**Create compressed archive:**

```bash
$ tar czf myarchive.tar.gz file1 file2 file3
# Creates compressed archive in one step

$ ls
myarchive.tar.gz  file1  file2  file3
```

**Extract compressed archive:**

```bash
$ tar xzf myarchive.tar.gz
# Extracts and decompresses in one step

$ ls
myarchive.tar.gz  file1  file2  file3
```

**Additional tar flags:**

- `z`: Use gzip compression/decompression
- `j`: Use bzip2 compression/decompression

**Memory aid**: "eXtract all Zee Files!" (xzf)

### Other Archive Types

**Common formats you might encounter:**

```bash
# bzip2 compression
$ bzip2 filename
$ bunzip2 filename.bz2

# ZIP archives
$ zip archive.zip file1 file2 file3
$ unzip archive.zip

# View archive contents without extracting
$ tar tzf archive.tar.gz    # List contents of compressed tar
$ tar tvf archive.tar       # List contents of tar archive
```

## 4. Package Dependencies

**Definition**: Additional packages or libraries required for a package to function properly.

### Dependency Relationship Example

```txt
Main Package: LibreOffice
├── Dependency: Java Runtime Environment (JRE)
├── Dependency: GTK+ Libraries
├── Dependency: FontConfig
└── Dependency: Various system libraries
```

### Types of Dependencies

- **Package dependencies**: Other software packages
- **Library dependencies**: Shared code libraries
- **System dependencies**: Core system components

**Example dependency chain:**

```bash
# Example: Installing a media player might require:
vlc → libvlc → ffmpeg → libx264 → various codecs
```

### Dependency Problems

```bash
# Without proper dependency management:
$ dpkg -i some-package.deb
dpkg: dependency problems prevent configuration of some-package:
 some-package depends on libexample1 (>= 1.2.3); however:
  Package libexample1 is not installed.
```

## 5. Low-level Package Management - rpm and dpkg

**Definition**: Basic package management tools that handle individual package files but don't resolve dependencies automatically.

### dpkg (Debian Package Manager)

**Install package:**

```bash
$ sudo dpkg -i google-chrome-stable_current_amd64.deb
Selecting previously unselected package google-chrome-stable.
(Reading database ... 185432 files and directories currently installed.)
Preparing to unpack google-chrome-stable_current_amd64.deb ...
Unpacking google-chrome-stable (91.0.4472.164-1) ...
Setting up google-chrome-stable (91.0.4472.164-1) ...
```

**Remove package:**

```bash
$ sudo dpkg -r google-chrome-stable
(Reading database ... 185440 files and directories currently installed.)
Removing google-chrome-stable (91.0.4472.164-1) ...
```

**List installed packages:**

```bash
$ dpkg -l
Desired=Unknown/Install/Remove/Purge/Hold
| Status=Not/Inst/Conf-files/Unpacked/halF-conf/Half-inst/trig-aWait/Trig-pend
|/ Err?=(none)/Reinst-required (Status,Err: uppercase=bad)
||/ Name           Version      Architecture Description
+++-==============-============-============-=================================
ii  bash           5.0-6ubuntu1 amd64        GNU Bourne Again SHell
ii  coreutils      8.30-3ubuntu2 amd64       GNU core utilities
```

**Query specific package:**

```bash
$ dpkg -l | grep chrome
ii  google-chrome-stable  91.0.4472.164-1  amd64  The web browser from Google
```

**Get package information:**

```bash
$ dpkg -s google-chrome-stable
Package: google-chrome-stable
Status: install ok installed
Priority: optional
Section: web
Installed-Size: 272748
Maintainer: Chrome Linux Team <chromium-dev@chromium.org>
Architecture: amd64
Version: 91.0.4472.164-1
```

### rpm (Red Hat Package Manager)

**Install package:**

```bash
$ sudo rpm -i firefox-91.0-1.fc34.x86_64.rpm
Preparing...                          ################################# [100%]
Updating / installing...
   1:firefox-91.0-1.fc34              ################################# [100%]
```

**Remove package:**

```bash
sudo rpm -e firefox
```

**List all installed packages:**

```bash
$ rpm -qa
firefox-91.0-1.fc34.x86_64
bash-5.1.8-2.fc34.x86_64
coreutils-8.32-30.fc34.x86_64
```

**Query specific package:**

```bash
$ rpm -qa | grep firefox
firefox-91.0-1.fc34.x86_64
```

**Get package information:**

```bash
$ rpm -qi firefox
Name        : firefox
Version     : 91.0
Release     : 1.fc34
Architecture: x86_64
Install Date: Wed 28 Jul 2021 10:30:45 AM EDT
Group       : Applications/Internet
Size        : 235891567
License     : MPLv1.1 or GPLv2+ or LGPLv2+
Signature   : RSA/SHA256, Mon 12 Jul 2021 01:23:45 PM EDT
Source RPM  : firefox-91.0-1.fc34.src.rpm
Build Date  : Mon 12 Jul 2021 12:45:23 PM EDT
Build Host  : buildvm-x86-01.iad2.fedoraproject.org
Packager    : Fedora Project
Vendor      : Fedora Project
URL         : https://www.mozilla.org/firefox/
Summary     : Mozilla Firefox Web browser
Description : Mozilla Firefox is an open-source web browser, designed for standards compliance, performance and portability.
```

## 6. High-level Package Management - yum and apt

**Definition**: Advanced package management systems that handle dependencies automatically.

### apt (Advanced Package Tool) - Debian Family

**Update package repository information:**

```bash
$ sudo apt update
Hit:1 http://archive.ubuntu.com/ubuntu focal InRelease
Get:2 http://archive.ubuntu.com/ubuntu focal-updates InRelease [114 kB]
Get:3 http://archive.ubuntu.com/ubuntu focal-backports InRelease [101 kB]
Get:4 http://security.ubuntu.com/ubuntu focal-security InRelease [114 kB]
Fetched 329 kB in 2s (164 kB/s)
Reading package lists... Done
```

**Install package:**

```bash
$ sudo apt install firefox
Reading package lists... Done
Building dependency tree       
Reading state information... Done
The following additional packages will be installed:
  firefox-locale-en libxul0d
The following NEW packages will be installed:
  firefox firefox-locale-en libxul0d
0 upgraded, 3 newly installed, 0 to remove and 0 not upgraded.
Need to get 54.7 MB of archives.
After this operation, 196 MB of additional disk space will be used.
Do you want to continue? [Y/n] y
```

**Remove package:**

```bash
$ sudo apt remove firefox
Reading package lists... Done
Building dependency tree       
Reading state information... Done
The following packages will be REMOVED:
  firefox firefox-locale-en
0 upgraded, 0 newly installed, 2 to remove and 0 not upgraded.
After this operation, 196 MB disk space will be freed.
Do you want to continue? [Y/n] y
```

**Upgrade all packages:**

```bash
$ sudo apt upgrade
Reading package lists... Done
Building dependency tree       
Reading state information... Done
Calculating upgrade... Done
The following packages will be upgraded:
  libc6 libc6-dev libc-dev-bin
3 upgraded, 0 newly installed, 0 to remove and 0 not upgraded.
Need to get 9,463 kB of archives.
After this operation, 0 B of additional disk space will be used.
Do you want to continue? [Y/n] y
```

**Search for packages:**

```bash
$ apt search text-editor
Sorting... Done
Full Text Search... Done
gedit/focal 3.36.2-1 amd64
  official text editor of the GNOME desktop environment

vim/focal 2:8.1.2269-1ubuntu5 amd64
  Vi IMproved - enhanced vi editor
```

**Show package information:**

```bash
$ apt show firefox
Package: firefox
Version: 91.0+build2-0ubuntu1
Priority: optional
Section: web
Origin: Ubuntu
Maintainer: Ubuntu Mozilla Team <ubuntu-mozillateam@lists.ubuntu.com>
Bugs: https://bugs.launchpad.net/ubuntu/+filebug
Installed-Size: 196 MB
Depends: libc6 (>= 2.17), libxul0d (= 91.0+build2-0ubuntu1)
Homepage: https://www.mozilla.org/firefox/
Description: Mozilla Firefox web browser
```

### yum (Yellowdog Updater Modified) - Red Hat Family

**Update package repository and upgrade packages:**

```bash
$ sudo yum update
Loaded plugins: fastestmirror, langpacks
Loading mirror speeds from cached hostfile
 * base: mirror.example.com
 * extras: mirror.example.com
 * updates: mirror.example.com
Resolving Dependencies
--> Running transaction check
---> Package firefox.x86_64 0:68.12.0-1.el7.centos will be updated
---> Package firefox.x86_64 0:91.0-1.el7.centos will be an update
--> Finished Dependency Resolution
```

**Install package:**

```bash
$ sudo yum install firefox
Loaded plugins: fastestmirror, langpacks
Loading mirror speeds from cached hostfile
Resolving Dependencies
--> Running transaction check
---> Package firefox.x86_64 0:91.0-1.el7.centos will be installed
--> Processing Dependency: mozilla-filesystem for package: firefox-91.0-1.el7.centos.x86_64
--> Running transaction check
---> Package mozilla-filesystem.x86_64 0:1.9-11.el7 will be installed
--> Finished Dependency Resolution
```

**Remove package:**

```bash
$ sudo yum erase firefox
Loaded plugins: fastestmirror, langpacks
Resolving Dependencies
--> Running transaction check
---> Package firefox.x86_64 0:91.0-1.el7.centos will be erased
--> Finished Dependency Resolution
```

**Search for packages:**

```bash
$ yum search text-editor
Loaded plugins: fastestmirror, langpacks
Loading mirror speeds from cached hostfile
========================== N/S matched: text-editor ===========================
gedit.x86_64 : A text editor for GNOME
vim-enhanced.x86_64 : A version of the VIM editor which includes recent enhancements
```

**Get package information:**

```bash
$ yum info firefox
Loaded plugins: fastestmirror, langpacks
Loading mirror speeds from cached hostfile
Available Packages
Name        : firefox
Arch        : x86_64
Version     : 91.0
Release     : 1.el7.centos
Size        : 102 M
Repo        : updates/7/x86_64
Summary     : Mozilla Firefox Web browser
URL         : https://www.mozilla.org/firefox/
License     : MPLv1.1 or GPLv2+ or LGPLv2+
Description : Mozilla Firefox is an open-source web browser, designed for
            : standards compliance, performance and portability.
```

## 7. Compiling from Source Code

**Definition**: Building software directly from source code when pre-compiled packages aren't available.

### Prerequisites

**Install build tools:**

```bash
# Debian/Ubuntu
$ sudo apt install build-essential
Reading package lists... Done
Building dependency tree       
Reading state information... Done
The following NEW packages will be installed:
  build-essential dpkg-dev fakeroot g++ g++-9 gcc gcc-9 libc6-dev libdpkg-perl make
0 upgraded, 10 newly installed, 0 to remove and 0 not upgraded.

# Red Hat/CentOS/Fedora
$ sudo yum groupinstall "Development Tools"
```

### Basic Compilation Process

#### Step 1: Extract source code

```bash
$ tar -xzvf awesome-software-1.2.3.tar.gz
awesome-software-1.2.3/
awesome-software-1.2.3/README
awesome-software-1.2.3/INSTALL
awesome-software-1.2.3/configure
awesome-software-1.2.3/Makefile.in
awesome-software-1.2.3/src/
awesome-software-1.2.3/src/main.c

$ cd awesome-software-1.2.3
```

#### Step 2: Read documentation

```bash
$ cat README
AWESOME SOFTWARE v1.2.3
=======================

This software requires:
- GCC compiler
- libawesome-dev (>= 2.1)
- libcool-dev (>= 1.5)

To compile and install:
1. ./configure
2. make
3. make install

$ cat INSTALL
# Additional installation instructions...
```

#### Step 3: Configure build

```bash
$ ./configure
checking for gcc... gcc
checking whether the C compiler works... yes
checking for library dependencies... yes
checking for libawesome... yes
checking for libcool... yes
configure: creating ./config.status
config.status: creating Makefile
config.status: creating config.h
```

**Handle missing dependencies:**

```bash
$ ./configure
checking for gcc... gcc
checking for libawesome... no
configure: error: libawesome development package not found

# Install missing dependency
$ sudo apt install libawesome-dev
$ ./configure
# Now should succeed
```

#### Step 4: Compile the software

```bash
$ make
gcc -c -o main.o main.c
gcc -c -o utils.o utils.c
gcc -o awesome-software main.o utils.o -lawesome -lcool
```

#### Step 5: Install the software

```bash
$ sudo make install
install -d /usr/local/bin
install -m 755 awesome-software /usr/local/bin/
install -d /usr/local/share/man/man1
install -m 644 awesome-software.1 /usr/local/share/man/man1/
```

### Alternative: Using checkinstall

**Install checkinstall:**

```bash
sudo apt install checkinstall
```

**Use checkinstall instead of make install:**

```bash
$ sudo checkinstall
checkinstall 1.6.2, Copyright 2009 Felipe Eduardo Sanchez Diaz Duran
          This software is released under the GNU GPL.

*****************************************
**** Debian package creation selected ***
*****************************************

This package will be built according to these values:

0 -  Maintainer: [ user@hostname ]
1 -  Summary: [ Package created with checkinstall 1.6.2 ]
2 -  Name:    [ awesome-software ]
3 -  Version: [ 1.2.3 ]
4 -  Release: [ 1 ]
5 -  License: [ GPL ]
6 -  Group:   [ checkinstall ]
7 -  Architecture: [ amd64 ]
8 -  Source location: [ awesome-software-1.2.3 ]
9 -  Alternate source location: [  ]
10 - Requires: [  ]
11 - Provides: [ awesome-software ]
12 - Conflicts: [  ]
13 - Replaces: [  ]

Enter a number to change any of them or press ENTER to continue:

Installing with make install...
========================= Installation successful ==========================
Copying documentation directory...
./
./README
./INSTALL

Copying files to the temporary directory...Done.

Stripping ELF binaries and libraries...Done.

Compressing man pages...Done.

Building file list...Done.

Building Debian package...Done.

Installing Debian package...Done.

Erasing temporary files...Done.

Writing backup package...Done.

Deleting temp dir...Done.

**********************************************************************

 Done. The new package has been installed and saved to

 /home/user/awesome-software_1.2.3-1_amd64.deb

 You can remove it from your system anytime using:

      dpkg -r awesome-software

**********************************************************************
```

### Uninstallation

**With regular make install:**

```bash
$ sudo make uninstall
rm -f /usr/local/bin/awesome-software
rm -f /usr/local/share/man/man1/awesome-software.1
```

**With checkinstall (recommended):**

```bash
$ sudo dpkg -r awesome-software
(Reading database ... 185440 files and directories currently installed.)
Removing awesome-software (1.2.3-1) ...
```

## Summary Commands Quick Reference

### Package Installation

```bash
# Debian/Ubuntu
sudo apt update                    # Update package lists
sudo apt install package_name     # Install package
sudo apt remove package_name      # Remove package
sudo apt upgrade                   # Upgrade all packages
apt search keyword                 # Search for packages
apt show package_name             # Show package info

# Red Hat/CentOS/Fedora
sudo yum update                   # Update packages
sudo yum install package_name     # Install package
sudo yum erase package_name       # Remove package
yum search keyword                # Search for packages
yum info package_name            # Show package info
```

### Direct Package Installation

```bash
# Debian packages
sudo dpkg -i package.deb          # Install .deb package
sudo dpkg -r package_name         # Remove package
dpkg -l                          # List installed packages
dpkg -s package_name             # Show package status

# RPM packages
sudo rpm -i package.rpm           # Install .rpm package
sudo rpm -e package_name          # Remove package
rpm -qa                          # List all packages
rpm -qi package_name             # Show package info
```

### Archive Operations

```bash
# tar operations
tar cvf archive.tar files...      # Create archive
tar xvf archive.tar               # Extract archive
tar czf archive.tar.gz files...   # Create compressed archive
tar xzf archive.tar.gz            # Extract compressed archive

# gzip operations
gzip filename                     # Compress file
gunzip filename.gz                # Decompress file
```

### Source Compilation

```bash
# Standard compilation
sudo apt install build-essential  # Install build tools
tar -xzf package.tar.gz           # Extract source
cd package-directory              # Enter directory
./configure                       # Configure build
make                             # Compile
sudo make install                # Install

# With checkinstall (recommended)
sudo checkinstall                # Install and create package
sudo dpkg -r package_name        # Remove later
```
