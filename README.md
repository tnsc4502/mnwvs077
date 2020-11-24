MapleNight Wvs Project
=
About this project
----
This is an official-like server emulator of TWMS ver. 077. 

This server is based on C++ language and takes lua as the script engine(the official server relies on a lua-like engine for their script system too), [here](https://hackmd.io/-iQnMU8eSieu4s-QraftrA) to understand the difference between our implementation and OdinMS. 

We (do our best) to follow official-like function naming, coding styles, core implementations and script implementations, for descriptions of script functions, please refer to [here](https://hackmd.io/EkoGL7yFTnutUS8gNiXcKw).

Furthermore, the server maintains a (rudimentary) memory allocator to prevent memory fragmentation. We also provide a manual which instruct developer to allocate/deallocate objects in this project correctly, please refer to [this](https://hackmd.io/mMCvnLFRTx2VrZTuot0OpQ).

How to build
----
Since the project currently relies on some Windows-Platform only stuffs such as ppltask(which is provided by Windows as their unique concurrent C++ API, in this project we use it to implement timer), the project can only be compiled under Windows env(Windows 7/10 were both tested).

This project by far is mostly working under Visual Studio 2015(sometimes on 2017 in different working env). To be compatible with newer version of VS, you may need to rebuild boost library.

The dependencies are included in the "Library" folder, which has been compressed as a rar file. All you need to do is to extract "boost_1_63_0" and "poco-1.7.8-all" folders under "Library". The former provides building blocks for our network core, and the later one provides access to the DataBase server.

The progress
---
100% (I hope so) client-server synchronized HitDamage/AttackDamage for players.
95%+ skills work.
Almost all multi-player elements such as Friend/Party/Guild/Whisper/EntrustedShop/Trading/Trunk systems are properly implemented.
Early stage party quests(Party1, Party2, Party3, Test-purposed Monster Carnival) are implemented(albet I only uploaded the Party1 and MC100 scripts).
Script functions to support official scripts are almost done(By doing some text replacement you could easily get the scripts work).
Cash items like ItemProtector/ItemNameChange/AvatarMega/ShopScanner... are almost implemented.
