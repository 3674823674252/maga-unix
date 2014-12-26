#!/usr/bin/env bash

set -e

# http://stackoverflow.com/questions/3976362/bash-scripts-requiring-sudo-password
if [[ $UID != 0 ]]
then
    echo "Please run this script with sudo:"
    echo "sudo $0 $*"
    exit 1
fi

if [ -f './512mlargefile' ]
then
  if [ -d "/mnt/512mfilesystem" ]
  then
    echo "Cleanup from the previous run (mount + file)"
    if mountpoint -q "/mnt/512mfilesystem/"
    then
      echo "Unmounting and removing dir.."
      sudo umount /mnt/512mfilesystem
      rmdir /mnt/512mfilesystem/
      sleep 1
    fi
    rm ./512mlargefile
  else
    echo "Cleanup from the previous run (file)"
    rm ./512mlargefile
    sleep 1
  fi
fi

echo "Creating 512 MB large file.."

dd if=/dev/zero bs=488M count=1 of=./512mlargefile
chmod 777 ./512mlargefile

echo "File created. Creating ext2 filesystem on it.."

sleep 1

sudo mke2fs -t ext2 -F ./512mlargefile

sleep 1

echo "File system created. Mounting it.."

sleep 1

if [ -d "/mnt/512mfilesystem" ]
then
  if mountpoint -q "/mnt/512mfilesystem"
  then
    echo "No need to mount anything, filesystem is there already!"
  else
    echo "It wasn't previously mounted, mounting it.."
    sleep 1
    sudo mount 512mlargefile /mnt/512mfilesystem/
    echo "Mounted!"
  fi
else
  echo "Creating /mnt/512mfilesystem to mount there.."
  mkdir /mnt/512mfilesystem/
  sleep 1
  sudo mount 512mlargefile /mnt/512mfilesystem/
  echo "Mounted!"
fi

sleep 1

echo "Going there to create folder hierarchy.."

sleep 1

cd /mnt/512mfilesystem/

if [ -d "./folder1" ]
then
  echo "Removing folder1 that was there already.."
  rmdir ./folder1
  sleep 1
fi

if [ -d "./file1" ]
then
  echo "Removing file1 that was there already.."
  rm file1
  sleep 1
fi

touch file1
mkdir folder1

if [ -d './lost+found' ]
then
  echo "Deleting lost+found dir!"
  sudo rmdir ./lost+found
  sleep 1
fi

echo "Done!"




