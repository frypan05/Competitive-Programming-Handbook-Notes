# 01 — Linux & Shell Scripting

---

## SECTION 1 — Process Management

**Q: What is the difference between a process and a thread?**

A: A process is an independent program in execution with its own memory space, file descriptors,
and PID. A thread is a lightweight unit of execution that shares memory and file descriptors with
its parent process. Context-switching between threads is cheaper than between processes.
In the kernel, both are represented by `task_struct`.

---

**Q: What does `fork()` do?**

A: `fork()` creates a child process that is an exact copy of the parent (copy-on-write). The
child gets a new PID. Both parent and child continue execution from the next instruction.
Return value: 0 in the child, child's PID in the parent, -1 on error.

---

**Q: What is a zombie process?**

A: A process that has finished execution but whose entry still exists in the process table because
the parent has not called `wait()` to collect the exit status. It holds no resources except the
PID and exit-status slot. Fix: make the parent call `wait()`, or kill the parent (init/PID-1
adopts orphans and reaps them automatically).

---

**Q: What is an orphan process?**

A: A process whose parent has died. The `init` process (PID 1, systemd) adopts it and calls
`wait()` to clean it up so it never becomes a zombie.

---

**Q: What are Linux signals? Name 5 important ones.**

```
SIGTERM (15) — graceful shutdown request. Can be caught/ignored by the process.
SIGKILL  (9) — immediate kill. CANNOT be caught, blocked, or ignored. Goes to kernel.
SIGHUP   (1) — terminal hangup. Conventionally used to reload config (e.g. nginx -s reload).
SIGINT   (2) — interrupt from keyboard (Ctrl+C). Can be caught.
SIGCHLD (17) — sent to parent when a child process stops or terminates.
```

---

**Q: How do you find and kill a process listening on port 8080?**

```bash
# Find the PID
lsof -i :8080
ss -tlnp | grep 8080
netstat -tlnp | grep 8080

# Kill it
kill -15 <PID>   # try graceful first
kill -9  <PID>   # force if it doesn't respond

# One-liner
fuser -k 8080/tcp
```

---

**Q: What is the difference between `kill -9` and `kill -15`?**

A: `kill -15` (SIGTERM) asks the process to terminate gracefully — it can catch this signal,
flush buffers, close connections, and exit cleanly. `kill -9` (SIGKILL) bypasses the process
entirely and is handled by the kernel directly; the process has zero chance to handle it.
Always try SIGTERM first; use SIGKILL only if the process does not respond.

---

## SECTION 2 — Filesystem & Permissions

**Q: Explain Linux file permissions. What does `chmod 755` mean?**

A: Permissions are represented as three triplets: owner | group | others.
Each triplet is r(4) + w(2) + x(1).
- 7 = 4+2+1 = rwx  (owner: read, write, execute)
- 5 = 4+0+1 = r-x  (group: read, execute)
- 5 = 4+0+1 = r-x  (others: read, execute)

Typical for binaries and publicly served directories.

---

**Q: What is the sticky bit?**

A: On a directory (`chmod +t`), only the file's owner, the directory's owner, or root can delete
or rename files inside it — even if others have write permission. Classic example: `/tmp`. Every
user can create files there but cannot delete each other's files.

---

**Q: What is setuid / setgid?**

A: setuid on an executable means it runs with the file owner's privileges, not the caller's.
Example: `/usr/bin/passwd` is setuid root so a regular user can change their own password.
setgid on a directory means new files created inside inherit the directory's group instead of
the creator's primary group. Useful for shared team directories.

---

**Q: What is an inode?**

A: A data structure on disk that stores metadata about a file: permissions, owner, group,
timestamps (atime/mtime/ctime), file size, and pointers to data blocks. The filename is NOT
stored in the inode — it lives in the directory entry, which maps a name to an inode number.
This is why hard links work: multiple filenames pointing to the same inode number.

---

**Q: Hard link vs Symbolic link — differences?**

```
Hard link:
  - Another directory entry pointing to the same inode.
  - Deleting the original file leaves the data intact (reference count > 0).
  - Cannot span filesystems.
  - Cannot link to directories (prevents cycles).

Symbolic (soft) link:
  - A separate file that stores a path string pointing to another file.
  - Has its own inode.
  - If the target is deleted, the symlink becomes dangling (broken).
  - Can span filesystems and can link directories.
```

---

## SECTION 3 — Memory & I/O

**Q: What is virtual memory?**

A: An abstraction where each process believes it has a large, contiguous address space. The
kernel uses page tables to map virtual addresses to physical RAM frames. Benefits: process
isolation (processes cannot access each other's memory), address spaces larger than physical
RAM via swap, and copy-on-write to avoid duplicating memory on fork().

---

**Q: What is swap? When is it bad?**

A: Swap is disk space used as an overflow when physical RAM is exhausted. The kernel moves
least-recently-used memory pages to swap (paging out) to free up RAM. It becomes a problem when
a system is heavily swapping ("thrashing") because disk I/O is orders of magnitude slower than
RAM, causing the system to become unresponsive.
Monitor with: `vmstat 1`, `free -h`, `sar -r`, `iotop`.

---

**Q: What do `buffers` and `cached` mean in `free -h`?**

```
Buffers — kernel buffers for raw block device I/O (filesystem metadata like inodes, dentries).
Cached  — page cache: recently read file data kept in RAM for fast re-access.

Both count as "available" memory. The kernel reclaims them when a process needs more RAM.
"Used" memory on Linux is rarely an emergency — check "available" instead.
```

---

## SECTION 4 — Networking Commands

**Q: What does `ss -tlnp` show?**

A: All listening TCP sockets with numeric addresses and the PID/program name.
(-t TCP, -l listening, -n numeric, -p process)
This is the modern, faster replacement for `netstat -tlnp`.
`ss` reads from the kernel via netlink instead of parsing `/proc/net`.

---

**Q: How do you trace the route to a remote host?**

```bash
traceroute google.com    # uses UDP probes by default on Linux
tracepath  google.com    # similar, does not require root
mtr        google.com    # continuous real-time traceroute, best for debugging
```

---

## SECTION 5 — Shell Scripting

**Q: What does `set -euo pipefail` do and why is it a best practice?**

```
set -e          — exit immediately if any command exits with a non-zero status code.
set -u          — treat unset or unbound variables as an error and exit.
                  Catches typos like $DATABSE_URL instead of $DATABASE_URL.
set -o pipefail — the return value of a pipeline is the exit status of the last command
                  that exited with a non-zero status. Without this:
                  cat missing_file | grep pattern   exits 0 even though cat failed!
```

Always place at the top of every production shell script.

---

**Q: What is the difference between `$@` and `$*` in bash?**

A: Both represent all positional parameters (script arguments).
- `"$@"` expands to each argument as a separate quoted string: `"$1" "$2" "$3"`
- `"$*"` expands to all arguments joined into a single string: `"$1 $2 $3"`

Use `"$@"` when forwarding arguments to other commands to preserve arguments that contain spaces.

---

**Q: What is the difference between `[ ]` and `[[ ]]`?**

```
[ ]   — POSIX sh /usr/bin/test command. Works in all POSIX shells.
        Variables must be quoted to handle empty strings and spaces.

[[ ]] — bash keyword (built-in). More powerful:
          - Supports =~ for regex matching
          - Variables do not need quoting inside
          - Supports && and || directly inside
          - Cannot be used in sh scripts
        Preferred in bash scripts.
```

---

**Q: How do you safely handle an unset variable with a default?**

```bash
VALUE="${VAR:-default_value}"           # use default if VAR is unset or empty
VALUE="${VAR:?Error: VAR must be set}"  # exit with error message if VAR is unset
VALUE="${VAR:+override}"                # use override only if VAR is set (rare)
```

---

**Q: What is process substitution `<(command)`?**

A: Runs a command and presents its output as if it were a temporary file (via a named pipe or
/dev/fd). Useful when a command expects a filename argument but you want to feed it a stream.

```bash
# Compare sorted versions of two files without creating temp files
diff <(sort file1.txt) <(sort file2.txt)

# Feed output of a command to while read
while IFS= read -r line; do
    echo "Processing: $line"
done < <(docker ps --format "{{.Names}}")
```

---

## SECTION 6 — Shell Scripts (Write These From Memory)

### Disk usage monitor — alert if any filesystem exceeds 80%

```bash
#!/bin/bash
set -euo pipefail

THRESHOLD=80

check_disk() {
    df -h | awk 'NR>1 {
        gsub(/%/, "", $5)
        if ($5+0 > '"$THRESHOLD"') print $0
    }'
}

RESULT=$(check_disk)

if [ -n "$RESULT" ]; then
    echo "DISK ALERT: Usage above ${THRESHOLD}%"
    echo "$RESULT"
    exit 1
fi

echo "All filesystems OK (below ${THRESHOLD}%)"
```

---

### Log rotation — keep last 5 versions

```bash
#!/bin/bash
set -euo pipefail

LOG_FILE="${1:?Usage: $0 <logfile>}"
MAX_VERSIONS=5

[ -f "$LOG_FILE" ] || { echo "File not found: $LOG_FILE"; exit 1; }

# Rotate: 4->5, 3->4, 2->3, 1->2
for i in $(seq $((MAX_VERSIONS-1)) -1 1); do
    [ -f "${LOG_FILE}.${i}" ] && mv "${LOG_FILE}.${i}" "${LOG_FILE}.$((i+1))"
done

mv "$LOG_FILE" "${LOG_FILE}.1"
touch "$LOG_FILE"
echo "Rotated: $LOG_FILE"
```

---

### Wait for a TCP port to become available

```bash
#!/bin/bash
set -euo pipefail

wait_for_port() {
    local host="$1"
    local port="$2"
    local max_attempts="${3:-30}"
    local delay="${4:-2}"
    local attempt=0

    echo "Waiting for ${host}:${port}..."
    until nc -z "$host" "$port" 2>/dev/null; do
        attempt=$((attempt + 1))
        if [ "$attempt" -ge "$max_attempts" ]; then
            echo "ERROR: ${host}:${port} not available after ${max_attempts} attempts"
            exit 1
        fi
        echo "  Attempt ${attempt}/${max_attempts} — retrying in ${delay}s..."
        sleep "$delay"
    done
    echo "${host}:${port} is ready!"
}

wait_for_port "${DB_HOST:-localhost}" "${DB_PORT:-5432}"
wait_for_port "${REDIS_HOST:-localhost}" "${REDIS_PORT:-6379}"

exec "$@"
```

---

### Cleanup Docker images — keep latest N

```bash
#!/bin/bash
set -euo pipefail

REPO="${1:?Usage: $0 <repository> [keep_count]}"
KEEP="${2:-5}"

log() { echo "[$(date +%H:%M:%S)] $*"; }

mapfile -t IMAGE_IDS < <(
    docker images --format "{{.ID}}\t{{.CreatedAt}}" "$REPO" \
    | sort -k2 \
    | awk '{print $1}'
)

TOTAL=${#IMAGE_IDS[@]}
TO_DELETE=$(( TOTAL - KEEP ))

if [ "$TO_DELETE" -le 0 ]; then
    log "Only $TOTAL images, keeping all."
    exit 0
fi

log "Found $TOTAL images. Deleting oldest $TO_DELETE..."
for (( i=0; i<TO_DELETE; i++ )); do
    log "Removing: ${IMAGE_IDS[$i]}"
    docker rmi "${IMAGE_IDS[$i]}" || log "Warning: could not remove ${IMAGE_IDS[$i]}"
done

log "Done."
```

---

## SECTION 7 — "What happens when..." Questions

**Q: What happens when you type `ls -la` and press Enter?**

```
1. The shell tokenises "ls -la" and searches PATH directories for ls.
2. Found at /usr/bin/ls.
3. Shell calls fork() — a child process is created (copy of the shell).
4. Child calls execve("/usr/bin/ls", ["ls", "-la"], envp).
   This replaces the child's process image with the ls binary.
5. ls opens the current directory via openat() and reads entries via getdents64().
6. ls writes formatted output to STDOUT (file descriptor 1).
7. ls exits with status 0.
8. Parent shell calls wait(), collects exit status, shows prompt again.
```

---

**Q: What is the `/proc` filesystem?**

A: A virtual filesystem (exists only in RAM, not on disk) that exposes kernel and process
information as readable files. Tools like `ps`, `top`, and `netstat` all read from /proc.

```
/proc/<PID>/status    — process state, memory usage, thread count
/proc/<PID>/fd/       — open file descriptors (symlinks to actual files/sockets)
/proc/<PID>/cmdline   — the command that started the process
/proc/<PID>/environ   — environment variables of the process
/proc/meminfo         — detailed memory statistics
/proc/cpuinfo         — CPU model, cores, features
/proc/net/tcp         — TCP socket table (what netstat reads)
/proc/loadavg         — load averages (what uptime reads)
```

---

**Q: What is load average?**

A: The average number of processes in the run queue (running + waiting for CPU) over 1, 5, and
15 minutes. Shown by `uptime` and `top`.

```
On a single-core machine: load of 1.0 = 100% utilisation, no waiting.
On a 4-core machine:      load of 4.0 = 100% utilisation, no waiting.
                          load of 8.0 = every core is busy AND has 1 waiting process.

Rule of thumb: if load average > number of CPU cores persistently, investigate CPU bottleneck.
```

---

## SECTION 8 — systemd & Service Management

```bash
systemctl start   nginx          # start a service now
systemctl stop    nginx          # stop a service
systemctl restart nginx          # stop + start
systemctl reload  nginx          # reload config without downtime (sends SIGHUP)
systemctl enable  nginx          # start automatically on boot
systemctl disable nginx          # do not start on boot
systemctl status  nginx          # show status and recent log entries
systemctl is-active nginx        # exit 0 if active, non-zero otherwise (good for scripts)

journalctl -u nginx -f           # follow service logs in real time
journalctl -u nginx -n 200       # last 200 log lines
journalctl -u nginx --since "1 hour ago"
journalctl -u nginx --since "2024-01-01 00:00:00" --until "2024-01-02 00:00:00"
journalctl -p err -u nginx       # only error-level and above
```

---

## SECTION 9 — Quick Reference Commands

```bash
# Processes
ps aux                            # all running processes
ps aux | grep nginx               # find specific process
top / htop                        # real-time process monitor
strace -p <PID>                   # trace system calls of a running process
lsof -p <PID>                     # all files/sockets open by a process
lsof -i :8080                     # what process is using port 8080
pstree <PID>                      # process tree

# Disk
df -h                             # disk free per filesystem
du -sh /*  2>/dev/null            # size of each top-level directory
du -sh /var/* | sort -rh | head   # largest directories in /var
find / -size +100M -type f 2>/dev/null  # files larger than 100MB

# Memory
free -h                           # memory summary (check "available" column)
vmstat 1 5                        # 5 memory/cpu snapshots 1 second apart
cat /proc/meminfo                 # detailed memory breakdown

# Network
ss -tlnp                          # listening TCP sockets with process names
ss -s                             # socket summary statistics
ip addr show                      # interface IP addresses
ip route show                     # routing table
traceroute / mtr <host>           # trace network path
tcpdump -i eth0 -nn port 8080     # capture packets on port 8080
curl -v https://example.com       # HTTP request with full headers
curl -o /dev/null -s -w "%{http_code}\n" http://localhost:8080/health
```
