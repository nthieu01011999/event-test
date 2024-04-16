
#/bin/sh

filename=$1
vendor=$2
model=$3
firmwarename=$4
SoftWareVersion=$5
BuildDate=$6
BuildTime=$7

cat <<EOF >$filename
{
  "Vendor": "$vendor",
  "Model": "$model",
  "FirmwareName": "$firmwarename",
  "SoftWareVersion": "$SoftWareVersion",
  "SoftBuildTime": "$BuildDate $BuildTime"
}
EOF