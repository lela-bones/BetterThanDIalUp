#!/bin/bash

#Munge install script for Debian 7.11
#This assumes that Warewulf is already fully installed on your cluster
#you MUST run this bash file as root user or with sudo

apt-get install munge
dd if=/dev/urandom bs=1 count=1024 >/etc/munge/munge.key
chmod 0400 /etc/munge/munge.key
chown -R munge.munge /etc/munge
chown -R munge.munge /var/*/munge

chmod 0700 /etc/munge
chmod 0711 /var/lib/munge
chmod 0700 /var/log/munge
chmod 0755 /var/run/munge

/etc/init.d/munge restart

cp vnfs.conf /usr/local/etc/warewulf/vnfs.conf

cp /etc/apt/sources.list /srv/chroots/debian7/etc/apt/sources.list

cp mungechroot.sh /srv/chroots/debian7/mungechroot.sh
chroot /srv/chroots/debian7 ./mungechroot.sh
cp /etc/munge/munge.key /srv/chroots/debian7/etc/munge/munge.key

chown -R munge.munge /srv/chroots/debian7/etc/munge
chown -R munge.munge /srv/chroots/debian7/var/*/munge
chmod 0700 /srv/chroots/debian7/etc/munge
chmod 0711 /srv/chroots/debian7/var/lib/munge
chmod 0700 /srv/chroots/debian7/var/log/munge
chmod 0755 /srv/chroots/debian7/var/run/munge

wwvnfs --chroot /srv/chroots/debian7 --hybridpath=/vnfs
wwsh file import /etc/passwd
wwsh file sync
exit
