# NOTICE
This program is not working correctly on kernel versions 4.x on Intel. It might work on other GPUs but there is no such guarantee.

# customresolution
Custom resolution scaling

# Requirements
`xrandr`

# Current working

This program currently reads from `~/rescfg` 4 numbers, namely the width and height of the native resolution and that of the scaled resolution.

# Usage
  `customresolution --switch` sends a SIGUSR1 signal to all daemons it could get (should normally be only the one for the currently active screen; doesn't work well with multiple sessions of the same user or if running as `root`)

  `customresolution` without arguments kills all other instances of the daemon, then it renames itself into `--resdaemon`

  `customresolution --reload` sends a SIGUSR2 signal to all daemons to reread the configuration file `~/rescfg`
