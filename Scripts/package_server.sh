#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")") && . $ROOTDIR/var.sh

PACKAGE_DIR=$(PACKAGE_DIR)_server

"$UE_BUILD_UAT"                         \
   -ScriptsForProject="$UPROJECT_PATH"  \
   Turnkey                              \
   -command=VerifySdk                   \
   -platform=$BUILD_PLATFORM            \
   -UpdateIfNeeded                      \
   -EditorIO                            \
   -EditorIOPort=49968                  \
   -project="$UPROJECT_PATH"            \
   BuildCookRun                         \
   -nop4                                \
   -utf8output                          \
   -nocompileeditor                     \
   -skipbuildeditor                     \
   -cook                                \
   -project="$UPROJECT_PATH"            \
   -target=$PROJECT_NAME"Server"        \
   -unrealexe="$UE_EDITOR_CMD"          \
   -platform=$BUILD_PLATFORM            \
   -installed                           \
   -stage                               \
   -archive                             \
   -package                             \
   -build                               \
   -pak                                 \
   -iostore                             \
   -compressed                          \
   -prereqs                             \
   -archivedirectory="$(PACKAGE_DIR)"   \
   -CrashReporter                       \
   -server                              \
   -serverconfig="$BUILD_CONFIGURATION" \
   -nocompile                           \
   -nocompileuat

