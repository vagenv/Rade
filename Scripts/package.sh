#!/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")") && . $ROOTDIR/var.sh

# Create package
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
   -target=$PROJECT_NAME                \
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
   -archivedirectory="$PACKAGE_DIR"     \
   -CrashReporter                       \
   -clientconfig="$BUILD_CONFIGURATION" \
   -nocompile                           \
   -nocompileuat


# Create shortcuts
SCRIPT_PREFIX=$PACKAGE_DIR/$PROJECT_NAME
echo "#!/bin/sh
. $SCRIPT_PREFIX.sh -logPSO -clearPSODriverCache" > $SCRIPT_PREFIX"_PSO.sh"
echo "#!/bin/sh
. $SCRIPT_PREFIX.sh -statnamedevents -trace=cpu,gpu,frame,logbookmark,file,loadtime -StatCmds=\"startfile\"" > $SCRIPT_PREFIX"_TRACE.sh"
echo "#!/bin/sh
. $SCRIPT_PREFIX.sh -trace=default,memory -StatCmds=\"startfile\"" > $SCRIPT_PREFIX"_MEMORY.sh"

chmod +x $SCRIPT_PREFIX"_PSO.sh" $SCRIPT_PREFIX"_TRACE.sh" $SCRIPT_PREFIX"_MEMORY.sh"

