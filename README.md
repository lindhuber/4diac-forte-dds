# DDS Communication Layer for the Eclipse 4diac FORTE run-time environment

### Building FORTE with DDS Support
1. **Clone and Build the 4diac FORTE Runtime Environment**

   Begin by cloning the [4diac FORTE repository](https://github.com/eclipse-4diac/4diac-forte) and follow the provided instructions to build the runtime environment.

2. **Add the DDS Layer to FORTE**

   Clone the DDS layer into the `/src/com` directory of the FORTE source code and follow the setup instructions for the layer.

3. **Build the Complete Project**

   Compile FORTE along with the DDS layer to complete the setup.

For more detailed information on the build process, please refer to the [4diac FORTE repository](https://github.com/eclipse-4diac/4diac-forte).

## DDS Layer with FastDDS already installed

If you have FastDDS already installed on your system, CMake will recognize the library and link it to the DDS layer. No git submodules are required in this case.

## DDS Layer Setup
This repository contains submodules. To clone it, use the following command:
```bash
git clone --recurse-submodules https://github.com/lindhuber/4diac-forte-dds.git
```
If you have already cloned the repository, you can initialize the submodules with the following command:
```bash
git submodule update --init --recursive
```

To build the DDS layer, execute the following scripts depending on your operating system:
```bash
./setup_posix.sh	// linux
.\setup_win.ps1		// windows
```
These scripts build the FastDDS library that was cloned as a submodule.

The layer itself is built as part of the FORTE build process.



