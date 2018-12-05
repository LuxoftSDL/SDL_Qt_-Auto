#!/bin/bash
IMAGE_NAME=$1; shift
SDL_BASE_DIR=$1; shift
QTA=$1; shift

EX_CONTAINER=$(docker ps | grep "$IMAGE_NAME" | awk '{print $1}')
if [ ! -z "$EX_CONTAINER" ]
then
    docker attach $EX_CONTAINER; exit 0
fi

GIT_UNAME=$(git config --global user.name)
GIT_EMAIL=$(git config --global user.email)

docker run --rm -it \
    -e LOCAL_USER_ID=`id -u $USER` \
    -e DISPLAY=$DISPLAY \
    -e IMAGE_NAME=$IMAGE_NAME \
    -e GIT_AUTHOR_NAME="$GIT_UNAME" \
    -e GIT_COMMITTER_NAME="$GIT_UNAME" \
    -e GIT_AUTHOR_EMAIL="$GIT_EMAIL" \
    -e GIT_COMMITTER_EMAIL="$GIT_EMAIL" \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $SDL_BASE_DIR:/home/developer/sdl \
    -v $QTA:/opt/qta \
    -v ~/.ssh:/home/developer/.ssh \
    $IMAGE_NAME "$@"
