# Use a Windows base image if your executable requires Windows runtime
FROM mcr.microsoft.com/windows/nanoserver:ltsc2022

# Copy your executable to the container
COPY chase.exe /chase.exe

# Set the default command to run your executable
CMD ["C:\\chase.exe"]

