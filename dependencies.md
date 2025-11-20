# general

- `sudo usermod -aG dialout <username>`. Get the privilege to serial devices
- enable automount of udisks in system settings
- (optional) `sudo timedatectl set-local-rtc false`, `sudo systemctl enable --now systemd-timesyncd.service`. Get correct time on linux.

# Fedora

- arm-none-eabi-gcc-cs
- arm-none-eabi-newlib
- python3-pyserial
- python3-psutil
- python3-twisted
