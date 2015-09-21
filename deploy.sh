#!/usr/bin/env bash
if [ "$DEP_INSTALL" = "true" ]
then
	echo "Installing dependencies"
	make deploy
fi