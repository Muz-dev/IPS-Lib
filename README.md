# IPS-Lib
IPS-Lib is a Library that can be implemented into any C++ project to generate IPS patches.

IPS-Lib uses the IPS32 standard.

There is an example main.cpp in the root of the project that shows you how to create a patch with it. 

# How to use it
All you need to do is copy the .h file into the project library folder (Or wherever you put Libraries) and include it. This library does make use of keystone engine so make sure that you link that library into your build system.

Then in your cpp file, create a new object like shown in the exmaple. The fist param is the name of the file that is created. The resaon that this is called buildId is in refereal to the NSO build ID from a Nintendo Switch format. If your not modding the nintendo switch then you can make this whatever you want and that is what the output file will be called. The output param is the name of the output folder that your patch will to placed in, and the startOffset is the hex offset at the which the start label can be found in your target executable, this is added to the offset that you pass to createPatch to make sure that it lines up basec around the start label.

# Credits

[Keystone Engine](https://github.com/keystone-engine/keystone)
