#!/bin/bash

# Add local user
# Either use the LOCAL_USER_ID if passed in at runtime or
# fallback

USER_ID=${LOCAL_USER_ID:-9001}

useradd --shell /bin/bash -u $USER_ID -o -c "" -m developer --create-home
echo "developer ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

chown developer /home/developer
chgrp developer /home/developer

export HOME=/home/developer

printf "Starting with user developer, UID : $USER_ID \n\n"
echo   "    You may run sudo without password"
printf "    You may run GIU applications in container\n\n"
echo   "Environment variables : "
env
echo ""
[ -e /opt/startup/ ] && for script in /opt/startup/*; do bash $script ; done
echo ""
/usr/games/cowsay -f koala "Welcome to $IMAGE_NAME!"
sudo -E -u developer sh -c "$@"
