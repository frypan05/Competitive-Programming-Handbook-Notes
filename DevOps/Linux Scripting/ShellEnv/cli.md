# Linux Command Line Interface Notes

## 1. The Shell

**What is the shell?**

- A program that takes commands from keyboard and sends them to the operating system
- Terminal/Console programs launch shells for you
- Default shell: **bash** (Bourne Again shell)
- Other shells: ksh, zsh, tsch

**Shell Prompt Format:**

```sh
username@hostname:current_directory
pete@icebox:/home/pete $
```

- `$` indicates normal user using Bash/Bourne/Korn shell
- Don't type the `$` when entering commands

**Basic Command:**

```bash
echo Hello World
```

## 2. Navigation Commands

### pwd (Print Working Directory)

```bash
pwd
```

- Shows your current directory location
- Path starts from root directory (/)

### cd (Change Directory)

```bash
cd /home/pete/Pictures    # Absolute path
cd Hawaii                 # Relative path
```

**Path Types:**

- **Absolute path**: Starts from root (`/`) - e.g., `/home/pete/Desktop`
- **Relative path**: From current location - e.g., `taxes/`

**Navigation Shortcuts:**

- `.` - Current directory
- `..` - Parent directory (one level up)
- `~` - Home directory
- `-` - Previous directory

```bash
cd .     # Stay in current directory
cd ..    # Go up one level
cd ~     # Go to home directory
cd -     # Go to previous directory
```

### ls (List Directories)

```bash
ls                    # List current directory
ls /home/pete         # List specific directory
ls -a                 # Show all files (including hidden)
ls -l                 # Long format with details
ls -la                # Combine flags
```

**ls -l Output Details:**

- File permissions
- Number of links
- Owner name
- Owner group
- File size
- Last modification timestamp
- File/directory name

## 3. File Operations

### touch (Create Files)

```bash
touch mysuperduperfile
```

- Creates new empty files
- Updates timestamps on existing files

### file (Check File Type)

```bash
file banana.jpg
```

- Shows description of file contents
- Linux filenames don't need to match file type

### cat (Display File Contents)

```bash
cat dogfile birdfile
```

- Displays file contents
- Can combine multiple files
- Best for short content only

### less (View Large Files)

```bash
less /home/pete/Documents/text1
```

**Navigation in less:**

- `q` - Quit
- Arrow keys/Page Up/Down - Navigate
- `g` - Go to beginning
- `G` - Go to end
- `/search` - Search for text
- `h` - Help

## 4. File Management

### cp (Copy)

```bash
cp mycoolfile /home/pete/Documents/cooldocs
cp *.jpg /home/pete/Pictures          # Copy all .jpg files
cp -r Pumpkin/ /home/pete/Documents   # Copy directory recursively
cp -i mycoolfile /home/pete/Pictures  # Interactive (prompt before overwrite)
```

**Wildcards:**

- `*` - All characters or any string
- `?` - One character
- `[]` - Any character within brackets

### mv (Move/Rename)

```bash
mv oldfile newfile                    # Rename file
mv file2 /home/pete/Documents         # Move file
mv file_1 file_2 /somedirectory       # Move multiple files
mv directory1 directory2              # Rename directory
mv -i directory1 directory2           # Interactive
mv -b directory1 directory2           # Create backup
```

### mkdir (Make Directory)

```bash
mkdir books paintings                 # Create multiple directories
mkdir -p books/hemmingway/favorites   # Create subdirectories
```

### rm (Remove)

```bash
rm file1                              # Remove file
rm -f file1                           # Force remove (no prompts)
rm -i file                            # Interactive
rm -r directory                       # Remove directory recursively
rmdir directory                       # Remove empty directory
```

**⚠️ Warning:** Files removed with `rm` are permanently deleted!

## 5. Search and Help

### find (Search for Files)

```bash
find /home -name puppies.jpg          # Find by name
find /home -type d -name MyFolder     # Find directory by name
```

**File Types:**

- `d` - Directory
- `f` - Regular file

### help (Get Command Help)

```bash
help echo                             # Help for bash built-ins
echo --help                           # Help for other programs
```

### man (Manual Pages)

```bash
man ls
```

- Detailed documentation for commands
- Built into most Linux systems

### whatis (Brief Description)

```bash
whatis cat
```

- Provides brief description of commands
- Sourced from manual pages

## 6. Useful Features

### history (Command History)

```bash
history
```

- Shows previous commands
- `↑` arrow - Previous command
- `!!` - Run last command
- `Ctrl+R` - Reverse search through history

### clear (Clear Terminal)

```bash
clear
```

### Tab Completion

- Press `Tab` to autocomplete commands, files, directories
- Works when there's a unique match

### alias (Create Shortcuts)

```bash
alias foobar='ls -la'                 # Create alias
unalias foobar                        # Remove alias
```

**Permanent Aliases:**

- Add to `~/.bashrc` file for persistence after reboot

## 7. Exit Commands

```bash
exit                                  # Exit shell
logout                                # Alternative exit
```

## Key Concepts to Remember

1. **Everything in Linux is a file**
2. **File hierarchy starts from root directory (`/`)**
3. **Hidden files start with `.`**
4. **Use flags to modify command behavior**
5. **Tab completion saves time**
6. **Be careful with `rm` - deletions are permanent**
7. **Linux filenames don't need extensions**
8. **Commands can be combined with multiple flags**

## Common Flag Patterns

- `-a` - All/Show hidden
- `-l` - Long format/List details
- `-r` - Recursive (for directories)
- `-i` - Interactive (prompt before action)
- `-f` - Force (no prompts)
- `-p` - Parent/Create parent directories
