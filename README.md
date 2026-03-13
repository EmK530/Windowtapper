# [Windowtapper](https://www.youtube.com/watch?v=dbKezpk4izU)
Windowtapper is a recreation of the Wavetapper song by Frums using individual windows to represent each cube playing sound.

<img width="256" height="256" alt="Windowtapper Showcase" src="https://github.com/user-attachments/assets/5a040bc9-7ccf-4e87-8d3e-daed40a19aa1" />

## Recognize this idea?
That's great! This project was inspired by the [Windowtapper video](https://www.youtube.com/watch?v=mzoCbcC4P90) made by M_rld 3 years ago, which unfortunately was closed source.<br>
This isn't an attempt to recreate their version, the goal was to match the colors and patterns displayed in the original Wavetapper video.

## How do I try it?
I have 3 versions for you waiting in [Releases](https://github.com/EmK530/Windowtapper/releases).<br>

### windowtapper_wav.zip
This version is an attempt to produce an executable as tiny as possible, you will get the most quality out of this one but you have to extract a dependency file with the samples. There are two executables however.<br>
- <b>windowtapper.exe</b>
  - This is the smallest executable you can possibly get, it's compressed with [UPX](https://upx.github.io/)<br>but this may anger some antiviruses so if you have trouble switch to <b>windowtapper_unp.exe</b>
- <b>windowtapper_unp.exe</b>
  - An unpacked version of <b>windowtapper.exe</b> that should seem less suspicious to antiviruses.

### windowtapper_opus.exe
This version is completely standalone, requiring no dependencies but has about 3 MB of samples (719 of them!) packed into the executable.

## How to compile?
I don't have a lot of advice but I'll say this right here:<br>
<b>This is a Windows app! It does Windows things! I have no plans to port this to Linux but you may!</b>

I used mingw32-make to compile the app and the code should contain everything necessary to compile.<br>
The WAV version is compiled by default, to compile opus run `mingw32-make opus`.<br>
Make sure to run `mingw32-make clean` when switching build modes or there will be build issues from the cache!<br>
I don't have the opus library stored elsewhere on my computer, it's [in this repository](https://github.com/EmK530/Windowtapper/tree/master/libs).

## Special Thanks
yobnedaa's [Individual Blocks Playlist](https://www.youtube.com/watch?v=N5xQTOXLjeU&list=PLF2EaFd_A24sPeOnTFu8E__YBjn-2x-Pz)
