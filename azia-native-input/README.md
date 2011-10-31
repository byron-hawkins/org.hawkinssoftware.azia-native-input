Azia Native Input
-----------------

This module contains a small native windows library to acquire
OS hardware input messages. There are x86 and x64 versions.

#### Artifact 

An archive named **native-bin.zip** containing:

* native/
    * NativeInput.x86.dll
    * NativeInput.x64.dll

#### Build

The project is integrated with Maven at the packaging level, so
there is no native Maven build available. To build the binaries:

1. Install the free Microsoft SDK pack (available online)
2. Find and launch the "Windows SDK Command Prompt"
    * It should be in the "Start" menu under "Windows SDK"
    * If not, all mine does is start a **cmd.exe** with args:
        + /E:ON
		+ /V:ON 
		+ /T:0E 
		+ /K 
		+ "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd"        
3. cd to the **azia-native-input** project directory
4. Run **SetEnv.cmd** with the platform switch
    * /x86 *for 32-bit*
    * /x64 *for 64-bit*
5. Run the respective "compile.x__.bat" file

To package the binaries into a zip, execute the following Maven
goals in sequence:

1. `clean`
2. `package`
3. `org.apache.maven.plugins:maven-assembly-plugin:2.2.1:single`

#### Installation

It is recommended to allow Maven to find and unpack the native
binaries. 

* The [Scrap Menagerie POM][scrap-pom] gives an example
  of copying the binaries into the project build directory for the 
  convenience of running the app from Eclipse.
* The [Scrap Menagerie Distribution POM][scrap-dist-pom] gives an
  example of packaging the binaries into an executable bundle.
  
[scrap-pom]: https://github.com/byron-hawkins/org.hawkinssoftware.scrap-menagerie-app/blob/master/scrap-menagerie-app/pom.xml
[scrap-dist-pom]: https://github.com/byron-hawkins/org.hawkinssoftware.scrap-menagerie-dist/blob/master/scrap-menagerie-dist/pom.xml

#### Usage

The input is acquired and processed for application consumption
by the [NativeInput] class of [azia-input]. 

[NativeInput]: https://github.com/byron-hawkins/org.hawkinssoftware.azia-input/blob/master/azia-input/src/main/java/org/hawkinssoftware/azia/input/NativeInput.java
[azia-input]: https://github.com/byron-hawkins/org.hawkinssoftware.azia-input/blob/master/azia-input/README.md
