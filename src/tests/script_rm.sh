#!/bin/bash

if [ -d "./targets" ]; then 
rm -r ./targets
else echo "./targets does not exist"
fi