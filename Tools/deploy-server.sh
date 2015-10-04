#!/bin/sh

echo "Starting Server app deployment-------------------"

# needed directories
DEPLOY_DIR=$(pwd)
BUILD_DIR=../Jeopardy-build/Qt5.3/Release/JeopardyServerUI
RESOURCES_DIR=../Resources

# blow away the old server saved app
rm -rf JeopardyServer.app

echo "Start copying over the app bundle and all its resources"

# copy the app bundle to deploy directory
cp -f -R $BUILD_DIR/JeopardyServer.app $DEPLOY_DIR

# copy the needed resources into app bundle resources
cp -f $RESOURCES_DIR/Server.icns $DEPLOY_DIR/JeopardyServer.app/Contents/Resources
cp -f $RESOURCES_DIR/server_settings.data $DEPLOY_DIR/JeopardyServer.app/Contents/Resources

# copy over the info.plist into app bundle
cp -f $RESOURCES_DIR/Server_Info.plist $DEPLOY_DIR/JeopardyServer.app/Contents/Info.plist

echo "Finished with copying"

echo "Running macdeployqt..."

#Dynamically attach all needed Qt libraries to the app
macdeployqt $DEPLOY_DIR/JeopardyServer.app

echo "All Done------------------------------------"