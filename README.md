# ODIS → ODV Generic Spreadsheet Profile

This repository defines an **ODIS-compatible profile** and supporting specifications for the **automatic generation of ODV Generic Spreadsheets** from provider-supplied tabular ocean data.

The goal is to allow ocean data producers to publish **simple datasets plus structured metadata**, from which valid ODV spreadsheets can be generated deterministically.

---

## Motivation

Many ocean data providers publish tabular data (CSV/TSV) but do not natively produce ODV files.

ODV, however, requires:
- strict column labels,
- mandatory meta-variables,
- specific formatting rules.

This project bridges that gap by using:

- **schema.org / JSON-LD** for discovery (ODIS-compatible)
- a small **`odv:` extension vocabulary** for executable conversion rules
- **JSON Schema** for validation and robustness

---

## Repository Contents

├── README.md
├── profile/
│ └── odis-profile-odv-generic-spreadsheet.md
├── vocabulary/
│ └── odv-vocabulary-spec.txt
├── schema/
│ └── odv-odis2odv.schema.json
└── examples/
└── ocean-data-test-001.json


### `profile/`
ODIS Book–style profile description defining the pattern and its scope.

### `vocabulary/`
Human-readable specification of the `odv:` extension vocabulary.

### `schema/`
JSON Schema used to validate dataset metadata files before conversion.

### `examples/`
Minimal, working JSON-LD examples that:
- pass schema validation
- can drive an ODV Generic Spreadsheet generator

---

## Core Concepts

### Discovery vs Execution

This project deliberately separates concerns:

| Layer | Purpose |
|------|--------|
| **schema.org** | Dataset discovery and indexing (ODIS) |
| **odv:** vocabulary | Deterministic ODV generation rules |

ODIS harvesters can index datasets without understanding `odv:`.

Converters can ignore discovery metadata and focus on execution.

---

## What This Enables

Using this profile, a dataset publisher can:

1. Publish a **simple tabular data file**
2. Publish a **JSON-LD metadata file** describing:
   - how to read the data
   - how to map columns to ODV
3. Automatically generate:
   - a valid **ODV Generic Spreadsheet**

No manual ODV formatting is required by the data provider.

---

## Validation Workflow

Before conversion, metadata files SHOULD be validated:

1. **JSON syntax**
2. **JSON Schema validation**
3. *(Optional)* JSON-LD expansion sanity check

Example (Python):

```python
import json
from jsonschema import validate

schema = json.load(open("schema/odv-odis2odv.schema.json"))
instance = json.load(open("examples/ocean-data-test-001.json"))

validate(instance=instance, schema=schema)
print("Metadata is valid")

## License

Documentation, specifications, schemas, and examples in this repository are
licensed under the **Creative Commons Attribution 4.0 International License (CC BY 4.0)**.

Software code (if added) is licensed under the **MIT License**, unless stated otherwise.
