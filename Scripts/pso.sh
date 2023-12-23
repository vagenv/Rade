#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")") && . $ROOTDIR/var.sh

# TODO: Replace prefix with build id 
PSO_PREFIX="psoprefix"
SHADER_FORMAT="SF_VULKAN_SM6"


USER_PSO_DIR=$PACKAGE_DIR/$PROJECT_NAME/Saved/CollectedPSOs
PROJECT_PSO_DIR=$PROJECT_DIR/Saved/Cooked/$OS_TARGET/$PROJECT_NAME/Metadata/PipelineCaches
ALL_PSO_DIR=$PROJECT_DIR/PSOCache

mkdir -p $ALL_PSO_DIR

# Copy user PSO
cp -r $USER_PSO_DIR/* $ALL_PSO_DIR/.

# Copy Engine PSO config
cp -r $PROJECT_PSO_DIR/* $ALL_PSO_DIR/.

# Package
$UE_EDITOR_CMD $PROJECT_NAME "$UPROJECT_PATH" \
    -run=ShaderPipelineCacheTools expand      \
    $ALL_PSO_DIR/*.rec.upipelinecache         \
    $ALL_PSO_DIR/*.shk                        \
    $ALL_PSO_DIR/$PSO_PREFIX$PROJECT$SHADER_FORMAT.spc

