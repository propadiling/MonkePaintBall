$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk -j 4
& adb push libs/arm64-v8a/libpaintball.so /sdcard/Android/data/com.AnotherAxiom.GorillaTag/files/mods/libpaintball.so
& adb shell am force-stop com.AnotherAxiom.GorillaTag
& adb shell am start com.AnotherAxiom.GorillaTag/com.unity3d.player.UnityPlayerActivity
& ./log.ps1