# ODIS → ODV Generic Spreadsheet Profile

This repository defines an **ODIS-compatible, schema.org-only profile** for the
**automatic generation of ODV Generic Spreadsheets** from provider-supplied
tabular ocean data (CSV / TSV).

The goal is to allow ocean data providers to publish **simple data files plus
structured, web-native metadata**, from which valid ODV Generic Spreadsheets
can be generated **deterministically and reproducibly**.

---

## Motivation

Many ocean data providers publish tabular data (CSV/TSV), but do not natively
produce ODV files.

ODV Generic Spreadsheets require:

- strict column labels,
- mandatory geographic metadata (longitude, latitude),
- specific conventions for variables and metadata.

This project bridges that gap by using:

- **schema.org / JSON-LD** for dataset discovery (ODIS-compatible),
- **schema.org-native mapping patterns** to describe column mappings,
- **JSON Schema** to validate mapping metadata before conversion.

No custom vocabularies are required.

---

## Design Principles

- **Schema.org only**  
  No custom namespaces or extension vocabularies are used.
- **Input-centric mapping**  
  Mappings are defined from *source columns* to *ODV output columns*.
- **Provider-friendly**  
  Metadata is readable, writable, and understandable by non-programmers.
- **ODIS-compatible**  
  Discovery metadata and conversion metadata coexist in one JSON-LD document.
- **Deterministic conversion**  
  Given the same input and metadata, the same ODV spreadsheet is produced.

---

## Repository Contents

├── examples
│   ├── ocean-data-test-003.json
│   └── ocean-data-test-003.txt
├── profile
│   └── odis-profile-odv-generic-spreadsheet.md
├── schema
│   └── odv-odis2odv.schema.json
└── README.md


---


### `profile/`
Human-readable profile description (ODIS Book style) defining scope,
semantics, and processing rules.

### `schema/`
JSON Schema for validating schema.org JSON-LD mapping documents.

### `examples/`
Minimal, working JSON-LD examples that:

- pass schema validation,
- can drive an ODV Generic Spreadsheet generator.

---

## Core Concept: Column Mapping via `variableMeasured`

Column mappings are expressed using **schema.org-native patterns**:

- `Dataset.variableMeasured[]`
- each entry is a `PropertyValue`

### Mapping model

For each source column:

- `PropertyValue.name`  
  → exact header of the **source column**
- `additionalProperty(name="targetColumn")`  
  → header of the **ODV output column**

Example:

```json
{
  "@type": "PropertyValue",
  "name": "Lon",
  "additionalProperty": [
    {
      "@type": "PropertyValue",
      "name": "targetColumn",
      "value": "Longitude [degrees_east]"
    }
  ]
}

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
