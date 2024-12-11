#!/usr/bin/env bash

ls examples | sort -R | head -n 1 | awk '{print "examples/"$1}' | xargs -I _ cp _ ./example.inkml
