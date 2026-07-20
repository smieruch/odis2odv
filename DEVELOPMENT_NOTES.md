# ODIS2ODV Development Notes

## Project Goal

Develop **`odisodv`**, a bidirectional Qt/C++ console application for
converting between **ODIS JSON-LD** and **ODV Generic Spreadsheet**.

Besides implementing the converter, the project is also used to learn
**modern C++** and the **Qt framework**. The implementation therefore
proceeds in small, well-understood steps while discussing design
decisions and software architecture.

------------------------------------------------------------------------

# General Principles

## Architecture

-   One executable: `odisodv`
-   Commands: `inspect`, `validate`, `convert`
-   Bidirectional converter from the beginning.
-   Internal format-independent data model.
-   Small incremental development steps.
-   Refactor only when the code naturally asks for it.
-   Prefer Qt classes where appropriate.
-   Use standard C++ where it is clearly the better solution.

------------------------------------------------------------------------

# Teaching Philosophy

The project also serves as a structured introduction to modern C++.

Focus on understanding

-   why an API is designed the way it is,
-   ownership and object lifetime,
-   software architecture,
-   Qt design principles,
-   modern C++ idioms.

Do not simply implement code. Discuss the design first and implement
afterwards.

------------------------------------------------------------------------

# Current Architecture

``` text
main()
│
├── parse command line
├── inspect
│   ├── readLocalFile()
│   ├── QJsonDocument::fromJson()
│   ├── QJsonObject root
│   ├── inspect JSON hierarchy
│   ├── DataDownload
│   └── contentUrl
├── validate
└── convert
```

------------------------------------------------------------------------

# Current Status

Completed

-   Qt console application
-   CMake project
-   Qt Creator project
-   Emacs + clangd
-   project.el
-   command parser
-   `inspect` command
-   local file loading
-   JSON parsing
-   extraction of the top-level JSON object
-   helper function: `getRequiredString()`
-   helper function: `getRequiredObject()`
-   helper function: `getRequiredArray()`
-   inspection of nested JSON structures
-   inspection of `DataDownload`
-   inspection of `distribution.contentUrl`

------------------------------------------------------------------------

# Current Helper Functions

``` cpp
QByteArray readLocalFile(...)
QString getRequiredString(...)
QJsonObject getRequiredObject(...)
QJsonArray getRequiredArray(...)
```

Each helper verifies the existence and type of a required JSON property
and returns the corresponding Qt type. Higher-level ODIS/ODV validation
is performed elsewhere.

------------------------------------------------------------------------

# High-Level Roadmap

1.  Command Line Interface ✔
2.  Resource Loading ✔
3.  JSON Inspection ← current
4.  DatasetDescription
5.  Metadata Validation
6.  Source Table Reader
7.  Structural Validation
8.  JSON-LD → ODV
9.  HOT Dataset
10. ODV → JSON-LD

------------------------------------------------------------------------

# Development Journal

## Milestone 3 -- JSON Inspection

### Goal

Learn Qt's JSON API and understand the structure of ODIS JSON-LD before
introducing higher-level abstractions.

### Implemented

-   `readLocalFile()`
-   `getRequiredString()`
-   `getRequiredObject()`
-   `getRequiredArray()`
-   inspection of `distribution`
-   inspection of the first `DataDownload`
-   inspection of `distribution.contentUrl`

### Design Decisions

-   Separate loading from parsing.
-   Separate parsing from validation.
-   Introduce helper functions only after repeated patterns emerge.
-   Keep helper functions focused on a single responsibility.
-   Follow the JSON-LD hierarchy in the code.
-   Keep variable names aligned with schema.org terminology.

### Qt Concepts Learned

-   `QJsonDocument`
-   `QJsonObject`
-   `QJsonArray`
-   `QJsonValue`
-   `isObject()`
-   `isArray()`
-   `toObject()`
-   `toArray()`

### Modern C++ Concepts

-   references
-   const references
-   object lifetime
-   helper-function design

### Debugging Techniques

-   Print `QJsonObject` and `QJsonArray` using
    `QJsonDocument::toJson()`.
-   Use Emacs `M-.` and `M-,` to navigate code.
-   Inspect intermediate JSON objects instead of guessing.
-   Read compiler diagnostics before changing code.

### Lessons Learned

-   A `QJsonArray` always returns `QJsonValue` elements.
-   `QJsonValue` is the generic wrapper around all JSON types.
-   `toObject()` converts a known object value into a `QJsonObject`.
-   JSON property names are case-sensitive (`contentUrl` ≠
    `contentURL`).
-   Small helper functions make parser code easier to read, test and
    debug.

------------------------------------------------------------------------

# Important Design Decisions

-   Separate loading from parsing.
-   Separate parsing from validation.
-   Separate validation from conversion.
-   Do not introduce classes too early.
-   Introduce classes when they naturally emerge from increasing
    complexity.
-   Prefer explicit, readable code over premature optimization.
-   Follow the JSON-LD hierarchy closely.
-   Keep variable names aligned with schema.org terminology.
-   Introduce helper functions only after repeated patterns emerge.

------------------------------------------------------------------------

# Immediate Next Steps

Continue the `inspect` command by traversing:

-   `variableMeasured[]`
-   `additionalProperty[]`
-   nested `PropertyValue` structures
