# Linux Boot Process - Complete Notes

## Overview

The Linux boot process consists of 4 main stages that occur from power-on to login:

1. **BIOS** - Hardware initialization and bootloader location
2. **Bootloader** - Kernel loading with parameters
3. **Kernel** - Device/memory initialization and init process startup
4. **Init** - Essential system services management

---

## Stage 1: BIOS (Basic Input/Output System)

### Purpose

- Initializes hardware components
- Performs Power-on Self Test (POST) to verify hardware integrity
- Locates and loads the bootloader

### Process

- **Boot Block Search**: BIOS searches for boot block on hard drive
- **Partition Table Check**: Looks at Master Boot Record (MBR) or GPT
- **MBR Location**: First sector of hard drive (first 512 bytes)
- **MBR Contents**: Code to load another program that loads the bootloader

### UEFI Alternative

- **Full Form**: Unified Extensible Firmware Interface
- **Purpose**: Modern successor to BIOS
- **Storage**: Uses .efi files stored on EFI system partition
- **Adoption**:
  - Macintosh machines use EFI booting
  - Windows has mostly moved to UEFI
  - GPT format designed for EFI use
- **Compatibility**: GPT disks include "protective MBR" for BIOS compatibility

---

## Stage 2: Bootloader

### Main Responsibilities

1. Boot into operating system (Linux or non-Linux)
2. Select kernel to use
3. Specify kernel parameters

### Common Bootloaders

- **GRUB** (most common for Linux)
- **LILO**
- **efilinux**
- **coreboot**
- **SYSLINUX**

### GRUB Boot Parameters

Access via GRUB menu using 'e' key during startup:

- **`initrd`** - Location of initial RAM disk
- **`BOOT_IMAGE`** - Kernel image location
- **`root`** - Root filesystem location
  - Example: `/dev/sda1` or UUID representation
- **`ro`** - Mounts filesystem in read-only mode
- **`quiet`** - Suppresses background boot messages
- **`splash`** - Enables splash screen display

---

## Stage 3: Kernel

### Initial Challenge

**Chicken-and-egg problem**: Kernel manages hardware but doesn't have all drivers available during bootup.

### Solution: Temporary Root Filesystem

#### Historical: initrd (Initial RAM Disk)

- Temporary root filesystem containing essential modules
- Kernel mounts initrd → loads bootup drivers → replaces with actual root filesystem

#### Modern: initramfs (Initial RAM Filesystem)

- Built into kernel itself
- Loads necessary drivers for real root filesystem
- No need to locate separate initrd file

### Root Filesystem Mounting Process

1. **Initial Mount**: Root partition mounted in **read-only mode**
2. **Integrity Check**: `fsck` runs safely to check system integrity
3. **Remount**: Root filesystem remounted in **read-write mode**
4. **Init Launch**: Kernel locates and executes init program

---

## Stage 4: Init

### Role

- First process started by kernel
- Starts all other essential system services
- Process ID (PID) = 1

### Three Major Init Implementations

#### 1. System V init (sysv)

- **Type**: Traditional init system
- **Method**: Sequential start/stop of processes
- **Control**: Uses startup scripts
- **States**: Machine state defined by runlevels
- **Behavior**: Each runlevel starts/stops machine differently

#### 2. Upstart

- **Usage**: Found on older Ubuntu installations
- **Concept**: Jobs and events based system
- **Method**: Starts jobs that perform actions in response to events
- **Architecture**: Event-driven process management

#### 3. Systemd

- **Status**: New standard for init
- **Approach**: Goal-oriented system
- **Method**: Defines goals and satisfies dependencies to complete them
- **Advantage**: Parallel service startup, faster boot times

---

## Key Concepts Summary

### Boot Sequence Flow

```txt
Power On → BIOS/UEFI → Bootloader → Kernel → Init → System Services
```

### Critical Files/Locations

- **MBR**: First 512 bytes of hard drive
- **EFI System Partition**: Contains .efi files for UEFI
- **Kernel Image**: Location specified in BOOT_IMAGE parameter
- **Root Filesystem**: Location specified in root parameter

### Modern vs Legacy

- **Legacy**: BIOS + MBR + initrd + SysV init
- **Modern**: UEFI + GPT + initramfs + systemd

### Troubleshooting Access Points

- **GRUB Menu**: Press 'e' during startup to view/edit parameters
- **Boot Parameters**: Modify kernel behavior during startup
- **Init Selection**: Different init systems for different distributions
