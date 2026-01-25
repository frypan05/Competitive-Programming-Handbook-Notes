# System Logging Notes

## 1. System Logging Overview

### What are System Logs?

- Services, kernel, daemons constantly generate data that gets saved as logs
- Provides human-readable journal of system events
- Logs are typically stored in `/var` directory (variable data location)

### How Logging Works

- **syslog service** sends information to the system logger
- **syslogd** daemon (or **rsyslogd** on newer distributions) handles the process:
  - Waits for event messages
  - Filters messages based on importance
  - Routes messages to files, console, or discards them

### Example Log Entry

```txt
Jan 27 07:41:32 icebox anacron[4650]: Job `cron.weekly' started
```

**Format breakdown:**

- `Jan 27 07:41:32` - Timestamp
- `icebox` - Hostname
- `anacron[4650]` - Service and process ID
- `Job 'cron.weekly' started` - Event description

---

## 2. syslog Configuration

### Modern Implementation

- **rsyslog** is the advanced version of syslog
- Used by most current Linux distributions
- Configuration files located in `/etc/rsyslog.d/`

### Configuration Example

From `/etc/rsyslog.d/50-default.conf`:

```txt
# First some standard log files. Log by facility.
auth,authpriv.*                 /var/log/auth.log
*.*;auth,authpriv.none          -/var/log/syslog
#cron.*                         /var/log/cron.log
#daemon.*                       -/var/log/daemon.log
kern.*                          -/var/log/kern.log
#lpr.*                          -/var/log/lpr.log
mail.*                          /var/log/mail.log
#user.*                         -/var/log/user.log
```

### Configuration Format

- **Left column:** Selector (what to log)
- **Right column:** Action (where to send logs)
- **Important:** Not all applications use rsyslog

### Manual Logging

Test logging with the logger command:

```bash
logger -s Hello
```

This entry will appear in `/var/log/syslog`

---

## 3. General Log Files

### Primary Log Files

#### `/var/log/messages`

- Contains all non-critical and non-debug messages
- Includes: bootup messages (dmesg), auth, cron, daemon logs
- **Use case:** General system activity overview

#### `/var/log/syslog`

- Logs everything except authentication messages
- **Use case:** Debugging system errors and troubleshooting

### Usage Strategy

These two files provide comprehensive system monitoring and are usually sufficient for most troubleshooting scenarios.

---

## 4. Kernel Logging

### `/var/log/dmesg`

- **Content:** Kernel ring buffer information from boot time
- **Includes:** Hardware drivers, kernel info, boot status
- **Behavior:** Gets reset on every boot
- **Use cases:** Hardware issues, boot problems
- **Alternative access:** Use `dmesg` command

### `/var/log/kern.log`

- **Content:** Kernel information and events
- **Includes:** dmesg output plus ongoing kernel events
- **Use cases:** Comprehensive kernel troubleshooting

---

## 5. Authentication Logging

### `/var/log/auth.log`

- **Content:** System authorization logs
- **Includes:** User logins, authentication methods

### Example Entry

```txt
Jan 31 10:37:50 icebox pkexec: pam_unix(polkit-1:session): session opened for user root by (uid=1000)
```

**Analysis:**

- User with UID 1000 opened a root session using pkexec
- Authentication method: pam_unix
- Useful for security auditing and login troubleshooting

---

## 6. Log Management with logrotate

### The Problem

- Log files consume significant disk space
- Need to balance log retention with storage efficiency
- Want to prioritize recent logs over old ones

### The Solution: logrotate

- **Purpose:** Automated log management utility
- **Execution:** Typically runs daily via cron
- **Configuration:** Files located in `/etc/logrotate.d/`

### Features

- Specify number of logs to retain
- Compress old logs to save space
- Automatic rotation based on size/time
- Customizable retention policies

### Alternative Tools

While other log rotation tools exist, logrotate is the most commonly used and widely supported solution.

---

## Key Takeaways

1. **Log Locations:** Most important logs are in `/var/log/`
2. **Primary Files:** `/var/log/syslog` and `/var/log/messages` for general troubleshooting
3. **Specialized Logs:** `/var/log/auth.log` for security, `/var/log/dmesg` for hardware
4. **Management:** Use logrotate for automated log maintenance
5. **Testing:** Use `logger` command to test log functionality
6. **Configuration:** Check `/etc/rsyslog.d/` for logging rules
