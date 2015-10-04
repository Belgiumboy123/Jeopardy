#!/bin/sh

echo "Starting deployment-------------------------"

# needed directories
DEPLOY_DIR=$(pwd)
BUILD_DIR=../Jeopardy-build/Qt5.3/Release/Jeopardy
RESOURCES_DIR=../Resources

# blow away the old saved app
rm -rf Jeopardy.app

echo "Start copying over the app bundle and all its resources"

# copy the app bundle to deploy directory
cp  -f -R $BUILD_DIR/Jeopardy.app $DEPLOY_DIR

# copy the needed resources into the app bundle resources
cp -f $RESOURCES_DIR/clues.db $DEPLOY_DIR/Jeopardy.app/Contents/Resources
cp -f $RESOURCES_DIR/song.mp3 $DEPLOY_DIR/Jeopardy.app/Contents/Resources
cp -f $RESOURCES_DIR/AppIcon.icns $DEPLOY_DIR/Jeopardy.app/Contents/Resources

# copy over the info.plist into app bundle
cp -f $RESOURCES_DIR/Info.plist $DEPLOY_DIR/Jeopardy.app/Contents

echo "Finished with copying"

echo "Running macdeployqt..."

# Dynamically attach all needed Qt libraries to the app
macdeployqt $DEPLOY_DIR/Jeopardy.app

echo "All Done------------------------------------"