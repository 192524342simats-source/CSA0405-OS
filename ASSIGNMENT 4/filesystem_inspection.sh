#!/usr/bin/env bash
# Task 3: Linux file-system inspection evidence. Run on the Linux machine being evaluated.
set -e
printf '=== FILE AND FILESYSTEM INSPECTION ===\n'
printf '\n[stat]\n'
stat .
printf '\n[df -i]\n'
df -i .
printf '\n[ls -i]\n'
ls -i . | head -20
printf '\n[filesystem type and mount]\n'
df -T .
DEVICE=$(df --output=source . | tail -1 | tr -d ' ')
case "$DEVICE" in
  /dev/*)
    if command -v tune2fs >/dev/null 2>&1; then
      printf '\n[tune2fs -l %s]\n' "$DEVICE"
      sudo tune2fs -l "$DEVICE" | grep -E 'Filesystem volume name|Filesystem features|Block size|Block count|Free blocks|Inode count|Free inodes' || true
    else
      echo 'tune2fs is not installed; install e2fsprogs or use dumpe2fs -h for ext filesystems.'
    fi
    ;;
  *) echo "Current path is on $DEVICE; tune2fs applies only to ext2/3/4 block devices." ;;
esac
