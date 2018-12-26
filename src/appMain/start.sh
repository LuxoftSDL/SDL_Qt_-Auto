#!/bin/bash
rm -r storage app_info.dat
LD_LIBRARY_PATH=./QTA_3rd_party/lib/:./QTA_3rd_party/x86_64/lib/:. ./smartDeviceLinkCore
