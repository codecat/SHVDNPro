Script Hook V .NET Pro
======================

This is a fork of Crosire's [ScriptHookVDotNet](https://github.com/crosire/scripthookvdotnet). The core has been rewritten to work with multiple real SHV script functions instead of only 1. The benefit of this is that you get a major performance boost due to the fact that we don't need to start our own threads and queue up natives on the main script thread, we can just call natives directly.

The fork was created for the [Grand Theft Multiplayer](https://gt-mp.net/) mod with the aim to boost performance.

## Requirements

* [C++ ScriptHook by Alexander Blade](http://www.dev-c.com/gtav/scripthookv/)
* [.NET Framework ≥ 4.6.1](https://www.microsoft.com/download/details.aspx?id=42642)
* [Visual C++ Redistributable for Visual Studio 2013 x64](https://www.microsoft.com/download/details.aspx?id=40784)

## Contributing

You'll need Visual Studio 2017 or higher to open the project file and the [Script Hook V SDK](http://www.dev-c.com/gtav/scripthookv/) extracted into "[/SDK](/SDK)".

To contribute, please contact GT-MP's Miss. This project is currently a non-public GT-MP project.

## License

All the source code except for the Vector, Matrix and Quaternion classes, which are licensed separately, is licensed under the conditions of the [zlib license](LICENSE.txt).
