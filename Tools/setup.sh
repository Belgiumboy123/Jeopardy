#!/bin/sh

echo "Please open up Qt Creator, go to Projects tab on the left hand pane."
echo "Update the build directory to workspace_root/Jeopardy-build/(Debug or Release)"
echo "Then build both Debug and Release mode."
echo "Note have not yet figured out how to automate the above steps."
echo "Copying over needed resouce files to both Debug and Release app bundles..."

RESOURCES_DIR=../Resources
BUILD_DIR_R=../Jeopardy-build/Release/Jeopardy
BUILD_DIR_D=../Jeopardy-build/Debug/Jeopardy
BUILD_DIR_R2=../Jeopardy-build/Qt5.3/Release/Jeopardy
BUILD_DIR_D2=../Jeopardy-build/Qt5.3/Debug/Jeopardy

cp -f $RESOURCES_DIR/clues.db $BUILD_DIR_R/Jeopardy.app/Contents/Resources
cp -f $RESOURCES_DIR/song.mp3 $BUILD_DIR_R/Jeopardy.app/Contents/Resources

cp -f $RESOURCES_DIR/clues.db $BUILD_DIR_D/Jeopardy.app/Contents/Resources
cp -f $RESOURCES_DIR/song.mp3 $BUILD_DIR_D/Jeopardy.app/Contents/Resources

cp -f $RESOURCES_DIR/clues.db $BUILD_DIR_D2/Jeopardy.app/Contents/Resources
cp -f $RESOURCES_DIR/song.mp3 $BUILD_DIR_D2/Jeopardy.app/Contents/Resources

cp -f $RESOURCES_DIR/clues.db $BUILD_DIR_R2/Jeopardy.app/Contents/Resources
cp -f $RESOURCES_DIR/song.mp3 $BUILD_DIR_R2/Jeopardy.app/Contents/Resources

echo "Done. Now should be able to launch and run app in Qt Creator."