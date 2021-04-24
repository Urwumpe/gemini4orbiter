# Contributing

# Branches

I tyrannically decided to use a rather boring branching scheme. Please make sure you use the correct branch as base for your contributions.

## Bug fixes to a release

Bugfixes for a release should be based on master branch. There will be no release branches for such maintenance. 

## New features or changes

Any new development should be based on the dev branch.

# contribution standards

- Any contribution should compile with Visual Studio 2019 Community. 
- There should be no absolute paths in project files. 
- The contribution should be compiled against the SDK of Orbiter 2016 (No beta version of Orbiter!).
- If possible, reference the issue you have worked on in your commit messages.
- Should we ever define a proper code style guide, please follow it. 

# Rudimentary Code style 

Please try to follow the code style as it was before for consistency.

- Tab for identation.
- Curly braces should be in their own line for beginning and ending function bodies, but can be at the end of the line inside functions.
- Type names use Pascal case.
- Method names use Pascal case.
- Variable names use camel case.

The historic sources used hungarian notation. For the sake of sane refactoring, don't use it in the future. C++ is strongly typed, use it.
