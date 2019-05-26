MapleNight Wvs Project
=
About this project
----
This is an official-like server emulator of TWMS ver. 077. 

This server is based on C++ language and uses lua as script language(the official server use a lua-like language for the script system also), [here](https://hackmd.io/-iQnMU8eSieu4s-QraftrA) to understand the difference between our implementation and OdinMS. 

We (do our best) to follow official-like function naming, coding styles, core implementations and script implementations, for descriptions of script functions, please refer to [here](https://hackmd.io/EkoGL7yFTnutUS8gNiXcKw).

Furthermore, the server maintains a memory allocator to prevent memory fragmentation. We also provide a manual which instruct developer to allocate/deallocate objects in this project correctly, please refer to [this](https://hackmd.io/mMCvnLFRTx2VrZTuot0OpQ).

How to build
----
This project currently is buildable only on Windows OS because we use ppltask(which is provided by Windows as their unique concurrent C++ API.) to implement timer and event task driver.

This project is working under Visual Studio 2015. To be compatible with newer version of VS, you may need to rebuild boost library under different version of VS.

The dependencies are included in the "Library" folder, which has been compressed as a rar file. All you need to do is to extract "boost_1_63_0" and "poco-1.7.8-all" folders under "Library" folder. The former is used for our network core, and the later one is to provide access to the MySQL server.

The progress
---
Multi-Player gaming is now supported(Friend, Party, Guild system are working properly).
Party Quest (Party1) had been implemented.
InGame Quests(which do not require any script), Shop, Trading, Trunk, Cash Shop, Personal Shop are now working bascially.
Monster stats, skills are working (basically).
Lots of basic functions for the script system have been implemented, it is now easy to extend.

