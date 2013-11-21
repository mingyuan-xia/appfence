adb shell mkdir /data/appfence
adb push libs/armeabi/appfence /data/appfence/appfence
adb shell chmod 777 /data/appfence/appfence
adb shell /data/appfence/appfence