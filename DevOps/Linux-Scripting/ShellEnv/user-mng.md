# Linux Users and Groups - Study Notes

## 1. Users and Groups Overview

### 1.1 Core Concepts

- **Purpose:** Access control and permissions management
- **User Identification:** System uses User ID (UID) internally, usernames are human-friendly
- **Group Identification:** System uses Group ID (GID) for group management
- **Home Directory:** Usually located at `/home/username`

### 1.2 Types of Users

- **Human Users:** Regular people using the system
- **System Users:** Daemon processes that run system services
- **Root/Superuser:** Most powerful user with access to everything

### 1.3 Permission Example

```bash
# Viewing a protected file without permissions
$ cat /etc/shadow
cat: /etc/shadow: Permission denied

# Check file permissions
$ ls -la /etc/shadow
-rw-r----- 1 root shadow 1134 Dec 1 11:45 /etc/shadow
# Breakdown: root owns file, shadow group can read, others have no access

# Access with sudo
$ sudo cat /etc/shadow
# Now displays file contents
```

**Example Output:**

```txt
root:$6$salt$hashedpassword...:18500:0:99999:7:::
daemon:*:18500:0:99999:7:::
bin:*:18500:0:99999:7:::
```

---

## 2. Root Access and Superuser Commands

### 2.1 sudo Command (Recommended)

```bash
# Run single command as root
$ sudo command_name

# Example: View system files
$ sudo cat /etc/shadow
$ sudo ls /root
$ sudo systemctl restart nginx
```

**Example sudo Session:**

```bash
# Regular user tries to access root directory
$ ls /root
ls: cannot open directory '/root': Permission denied

# Using sudo
$ sudo ls /root
Desktop  Documents  Downloads  # Shows contents
```

### 2.2 su Command (Switch User)

```bash
# Switch to root user (opens root shell)
$ su
Password: [enter root password]
# Now you're in root shell - be careful!

# Switch to specific user
$ su - username
Password: [enter user's password]

# Exit back to original user
$ exit
```

**Example su Session:**

```bash
john@computer:~$ su
Password: ********
root@computer:/home/john# whoami
root
root@computer:/home/john# exit
john@computer:~$ whoami
john
```

### 2.3 Sudoers File

```bash
# Edit sudoers file (always use visudo)
$ sudo visudo

# View current sudoers (don't edit directly)
$ sudo cat /etc/sudoers
```

**Example /etc/sudoers content:**

```txt
root    ALL=(ALL:ALL) ALL
%admin  ALL=(ALL) ALL
%sudo   ALL=(ALL:ALL) ALL
john    ALL=(ALL) NOPASSWD: /usr/bin/systemctl
```

---

## 3. /etc/passwd File

### 3.1 Viewing User Information

```bash
# View all users
$ cat /etc/passwd

# View specific user
$ grep username /etc/passwd

# Count total users
$ wc -l /etc/passwd
```

### 3.2 Understanding /etc/passwd Format

**Field Structure:** `username:password:UID:GID:GECOS:home_directory:shell`

**Example Entry:**

```txt
john:x:1001:1001:John Smith,Room 101,555-1234:/home/john:/bin/bash
```

**Field Breakdown:**

1. **Username:** `john`
2. **Password:** `x` (stored in /etc/shadow)
3. **UID:** `1001` (User ID)
4. **GID:** `1001` (Primary Group ID)
5. **GECOS:** `John Smith,Room 101,555-1234` (Real name, office, phone)
6. **Home Directory:** `/home/john`
7. **Shell:** `/bin/bash`

### 3.3 Special User Examples

```bash
# System users examples from /etc/passwd
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
www-data:x:33:33:www-data:/var/www:/usr/sbin/nologin
```

**Analysis:**

- `root`: UID 0 (superuser), has login shell
- `daemon`: UID 1, system user, no login shell
- `www-data`: Web server user, no login access

### 3.4 Password Field Symbols

- `x`: Password stored in /etc/shadow
- `*`: User has no login access
- `(blank)`: User has no password

**Example:**

```txt
john:x:1001:1001:John Smith:/home/john:/bin/bash      # Normal user
guest:*:1002:1002:Guest Account:/home/guest:/bin/bash  # No login
test::1003:1003:Test User:/home/test:/bin/bash         # No password
```

---

## 4. /etc/shadow File

### 4.1 Viewing Shadow File

```bash
# Requires sudo access
$ sudo cat /etc/shadow

# View specific user's shadow entry
$ sudo grep username /etc/shadow
```

### 4.2 Understanding /etc/shadow Format

**Field Structure:** `username:encrypted_password:last_change:min_age:max_age:warn_period:inactive_period:expiration_date:reserved`

**Example Entry:**

```txt
john:$6$randomsalt$longhashpassword:18500:0:99999:7:30:19000:
```

**Field Breakdown:**

1. **Username:** `john`
2. **Encrypted Password:** `$6$randomsalt$longhashpassword`
3. **Last Change:** `18500` (days since Jan 1, 1970)
4. **Min Age:** `0` (days before password can be changed again)
5. **Max Age:** `99999` (days before password must be changed)
6. **Warning Period:** `7` (days before expiration warning)
7. **Inactive Period:** `30` (days after expiration before account locks)
8. **Expiration Date:** `19000` (account expires on this date)
9. **Reserved:** Empty field for future use

### 4.3 Password Field Examples

```bash
# Active user with password
john:$6$salt$hash:18500:0:99999:7:::

# Locked account
mary:!$6$salt$hash:18500:0:99999:7:::

# No password set
guest:*:18500:0:99999:7:::

# Account disabled
disabled:!:18500:0:99999:7:::
```

### 4.4 Date Calculation Example

```bash
# Calculate days since Jan 1, 1970
$ date -d "1970-01-01 + 18500 days"
Mon Aug 15 00:00:00 UTC 2020

# Current date in days since epoch
$ echo $(( $(date +%s) / 86400 ))
```

---

## 5. /etc/group File

### 5.1 Viewing Group Information

```bash
# View all groups
$ cat /etc/group

# View specific group
$ grep groupname /etc/group

# View groups for current user
$ groups

# View groups for specific user
$ groups username
```

### 5.2 Understanding /etc/group Format

**Field Structure:** `group_name:password:GID:user_list`

**Example Entries:**

```txt
root:*:0:
sudo:*:27:john,mary
developers:*:1500:john,alice,bob
marketing:*:1501:mary,sarah
```

**Field Breakdown:**

1. **Group Name:** `developers`
2. **Password:** `*` (not used, sudo preferred)
3. **GID:** `1500` (Group ID)
4. **User List:** `john,alice,bob` (comma-separated)

### 5.3 Group Examples with Users

```bash
# View current user's groups
$ groups
john adm cdrom sudo dip plugdev lpadmin sambashare

# View specific user's groups
$ groups alice
alice : alice developers marketing

# View who's in sudo group
$ grep sudo /etc/group
sudo:x:27:john,mary,admin
```

### 5.4 Primary vs Secondary Groups

```bash
# Primary group (from /etc/passwd)
$ grep john /etc/passwd
john:x:1001:1001:John Smith:/home/john:/bin/bash
#               ^ Primary GID (1001)

# Secondary groups (from /etc/group)
$ grep john /etc/group
sudo:x:27:john,mary
developers:x:1500:john,alice
```

---

## 6. User Management Tools

### 6.1 Adding Users

#### useradd Command (Basic)

```bash
# Basic user creation
$ sudo useradd bob

# Create user with home directory
$ sudo useradd -m bob

# Create user with specific shell
$ sudo useradd -m -s /bin/bash bob

# Create user with specific UID
$ sudo useradd -m -u 1500 bob

# Create user with multiple groups
$ sudo useradd -m -G sudo,developers bob
```

#### adduser Command (Interactive)

```bash
# Interactive user creation (Debian/Ubuntu)
$ sudo adduser bob
Adding user `bob' ...
Adding new group `bob' (1002) ...
Adding new user `bob' (1002) with group `bob' ...
Creating home directory `/home/bob' ...
Copying files from `/etc/skel' ...
New password: 
Retype new password: 
passwd: password updated successfully
```

### 6.2 Removing Users

```bash
# Remove user (keep home directory)
$ sudo userdel bob

# Remove user and home directory
$ sudo userdel -r bob

# Remove user and all files
$ sudo userdel -r --force bob
```

**Example Deletion Process:**

```bash
# Before deletion
$ ls /home
alice  bob  john

$ grep bob /etc/passwd
bob:x:1002:1002:Bob User:/home/bob:/bin/bash

# Delete user
$ sudo userdel -r bob

# After deletion
$ ls /home
alice  john

$ grep bob /etc/passwd
# No output - user removed
```

### 6.3 Changing Passwords

```bash
# Change your own password
$ passwd
Changing password for john.
Current password: 
New password: 
Retype new password: 
passwd: password updated successfully

# Change another user's password (as root)
$ sudo passwd bob
New password: 
Retype new password: 
passwd: password updated successfully

# Force password change on next login
$ sudo passwd -e bob
```

### 6.4 User Modification

```bash
# Change user's shell
$ sudo usermod -s /bin/zsh john

# Add user to group
$ sudo usermod -a -G sudo john

# Change user's home directory
$ sudo usermod -d /home/newjohn john

# Lock user account
$ sudo usermod -L john

# Unlock user account
$ sudo usermod -U john
```

### 6.5 Checking User Creation Results

```bash
# Check /etc/passwd entry
$ grep bob /etc/passwd
bob:x:1002:1002::/home/bob:/bin/bash

# Check /etc/shadow entry
$ sudo grep bob /etc/shadow
bob:$6$randomsalt$hashedpassword:18500:0:99999:7:::

# Check home directory
$ ls -la /home/bob
total 20
drwxr-xr-x 2 bob  bob  4096 Dec  1 10:30 .
drwxr-xr-x 5 root root 4096 Dec  1 10:30 ..
-rw-r--r-- 1 bob  bob   220 Dec  1 10:30 .bash_logout
-rw-r--r-- 1 bob  bob  3771 Dec  1 10:30 .bashrc
-rw-r--r-- 1 bob  bob   807 Dec  1 10:30 .profile
```

---

## Quick Reference Commands

### User Information

```bash
whoami              # Current username
id                  # Current user ID and groups
groups              # Current user's groups
w                   # Who is logged in
last                # Last login history
```

### File Viewing

```bash
cat /etc/passwd     # All users
cat /etc/group      # All groups
sudo cat /etc/shadow # Password information
```

### User Management

```bash
sudo useradd -m username    # Add user with home
sudo userdel -r username    # Remove user and home
sudo passwd username        # Change password
sudo usermod -a -G group user # Add user to group
```

### Permission Commands

```bash
sudo command        # Run as root
su - username      # Switch to user
sudo visudo        # Edit sudoers file
```

---

## Security Best Practices

1. **Use sudo instead of su** for administrative tasks
2. **Always use visudo** to edit /etc/sudoers
3. **Never edit /etc/passwd, /etc/shadow, /etc/group directly** - use proper tools
4. **Use strong passwords** and consider password aging policies
5. **Remove unused user accounts** promptly
6. **Monitor sudo usage** through logs (/var/log/auth.log)
7. **Use groups for permission management** rather than individual user permissions
