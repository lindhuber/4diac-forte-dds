# PowerShell script to build and install FastDDS locally on Windows

Set-Location -Path "third_party/Fast-DDS"

# create a build directory if it doesn't exist
if (!(Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build"
}
Set-Location -Path "build"

cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX=../../fastdds_installed

cmake --build . --config Release --target INSTALL

# return to the project root
Set-Location -Path "../../.."
Write-Host "FastDDS built and installed in fastdds_installed directory."
