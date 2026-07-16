# ODIS2ODV Development Notes

## Project Goal

Develop **`odisodv`**, a bidirectional Qt/C++ console application for converting between **ODIS JSON-LD** and **ODV Generic Spreadsheet**.

Besides implementing the converter, the project is also used to learn **modern C++** and the **Qt framework**. The implementation therefore proceeds in small, well-understood steps while discussing design decisions and software architecture.

---

# General Principles

## Architecture

* One executable: `odisodv`
* Commands

  * `inspect`
  * `validate`
  * `convert`
* Bidirectional converter from the beginning.
* Internal format-independent data model.
* Small incremental development steps.
* Refactor only when the code naturally asks for it.
* Prefer Qt classes where appropriate.
* Use standard C++ where it is clearly the better solution.

---

# Teaching Philosophy

The project also serves as a structured introduction to modern C++.

Focus on understanding

* why an API is designed the way it is,
* ownership and object lifetime,
* software architecture,
* Qt design principles,
* modern C++ idioms.

Do not simply implement code. Discuss the design first and implement afterwards.

---

# Current Architecture

```text
main()
│
├── parse command line
│
├── inspect
│   │
│   ├── readLocalFile()
│   │       │
│   │       └── QByteArray
│   │
│   ├── QJsonDocument::fromJson()
│   │
│   └── QJsonObject root
│
├── validate
│
└── convert
```

---

# Current Status

Completed

* Qt console application
* CMake project
* Qt Creator project
* Emacs + clangd
* project.el
* command parser
* `inspect` command
* local file loading
* `QFile`
* `QTextStream`
* `QByteArray`
* JSON parsing
* JSON parse error handling
* extraction of the top-level JSON object

Current processing pipeline

```text
command line
        ↓
readLocalFile()
        ↓
QByteArray
        ↓
QJsonDocument
        ↓
QJsonObject
```

---

# Current Helper Function

```cpp
QByteArray readLocalFile(...)
```

Responsibilities

* open local file,
* read the file,
* return a `QByteArray`,
* report file I/O errors.

It intentionally does **not**

* parse JSON,
* know anything about ODIS,
* know anything about ODV.

Single responsibility

```text
file
   ↓
bytes
```

---

# High-Level Roadmap

## Phase 1 – Command Line Interface

Status: **Completed**

* command parsing
* help
* exit codes
* dispatch to commands

---

## Phase 2 – Resource Loading

Support

* local files
* HTTP URLs
* GitHub URLs

Output

```text
QByteArray
```

The metadata may reference a dataset via `distribution.contentUrl`. A command-line option should later allow overriding that URL for testing.

---

## Phase 3 – JSON Inspection

**Current phase**

Gradually inspect

* root object
* `@type`
* `name`
* `description`
* `distribution`
* `variableMeasured`
* `additionalProperty`

Goal

Understand the metadata structure before introducing additional abstractions.

---

## Phase 4 – DatasetDescription

Introduce a format-independent internal representation.

```text
JSON-LD
     │
     ▼
DatasetDescription
     │
     ├── ODV Writer
     └── JSON-LD Writer
```

This internal model becomes the central representation for both conversion directions.

---

## Phase 5 – Metadata Validation

Validate

* mandatory fields,
* mappings,
* `DataType`,
* variable roles,
* primary variable,
* related columns,
* semantic consistency.

---

## Phase 6 – Source Table Reader

Read CSV/TSV data using metadata

* separator,
* fill value,
* encoding (if needed later).

Validate

* header,
* mapped columns,
* row consistency.

---

## Phase 7 – Structural Validation

Validate assumptions about the source dataset.

The converter assumes that the source data follow a well-defined logical structure.

Examples

* profile datasets grouped by profile,
* time series grouped by station,
* observations ordered by the declared primary variable,
* contiguous observation groups.

The converter validates these assumptions but should not silently reorder data.

---

## Phase 8 – JSON-LD → ODV Conversion

Implement the complete conversion pipeline.

```text
JSON-LD
      ↓
metadata validation
      ↓
load source data
      ↓
table validation
      ↓
mapping
      ↓
ODV Generic Spreadsheet
```

---

## Phase 9 – HOT Dataset

Apply the converter to the complete HOT dataset and validate the implementation using a real-world example.

---

## Phase 10 – ODV → JSON-LD

Implement the reverse conversion.

```text
ODV
   ↓
ODV importer
   ↓
DatasetDescription
   ↓
JSON-LD writer
```

---

# Important Design Decisions

* Separate loading from parsing.
* Separate parsing from validation.
* Separate validation from conversion.
* Do not introduce classes too early.
* Introduce classes when they naturally emerge from increasing complexity.
* Keep responsibilities clearly separated.
* Prefer explicit, readable code over premature optimization.

---

# Modern C++ Topics Covered

* references
* pointers
* const references
* object lifetime
* constructors
* destructors
* RAII
* `QFile`
* `QTextStream`
* `QByteArray`
* `QJsonDocument`
* `QJsonObject`
* compiler diagnostics
* Emacs + clangd
* `project.el`

Additional modern C++ concepts should be introduced naturally during implementation rather than in isolation.

---

# Immediate Next Steps

Continue implementing the `inspect` command.

Read and inspect

* `@type`
* `name`
* `description`
* `distribution`
* `variableMeasured`

Only introduce new classes when the current implementation clearly becomes too large or repetitive.
