<!--
Licensed under CC BY 4.0
https://creativecommons.org/licenses/by/4.0/
-->

# ODIS Profile: ODV Generic Spreadsheet Mapping

**Profile ID:** odis-profile-odv-generic-spreadsheet  
**Status:** Draft  
**Version:** 0.3

---

## Overview

This profile defines a web-first pattern for publishing ocean datasets so they
can be automatically converted into ODV Generic Spreadsheets.

It uses:

- schema.org for dataset discovery and access
- schema.org-native patterns (`variableMeasured`, `PropertyValue`,
  `additionalProperty`) to describe deterministic conversion rules

The profile targets data providers publishing CSV/TSV style tabular datasets
and services that generate ODV-compatible outputs programmatically.

---

## Scope

### In scope

- Dataset landing pages with embedded JSON-LD
- Tabular provider data (CSV, TSV)
- Deterministic generation of ODV Generic Spreadsheets
- ODV collection metadata description
- Source-column to ODV-column mappings

### Out of scope

- Scientific quality control
- Quality flag semantics
- ODV binary collections
- NetCDF generation

---

## Alignment with ODIS Patterns

This profile follows ODIS conventions:

- `schema.org/Dataset` for discovery
- `distribution.contentUrl` for machine-accessible source files
- `variableMeasured` for variable descriptions
- `additionalProperty` as the schema.org extension mechanism

ODIS harvesters can index datasets without understanding the ODV conversion
metadata.

---

## Vocabularies Used

Only schema.org terms are required.

Core terms:

- Dataset
- DataDownload
- PropertyValue
- distribution
- contentUrl
- variableMeasured
- additionalProperty

No custom vocabulary is required.

---

## Required Dataset Metadata

A conforming dataset MUST provide:

- `@type: Dataset`
- `@id`
- `identifier`
- `name`
- `description`
- `distribution`
- `distribution.contentUrl`
- `variableMeasured`

Longitude and latitude mappings are mandatory for successful ODV conversion.

---

## Dataset-Level ODV Metadata and Conversion Hints

Dataset-level information is encoded as:

`Dataset.additionalProperty[]`

Each entry is a schema.org `PropertyValue`.

### ODV Collection Metadata

#### DataField

Defines the ODV scientific domain.

Example:

```json
{
  "@type": "PropertyValue",
  "name": "DataField",
  "value": "Ocean"
}
```

#### DataType

Defines the ODV data organization.

Supported examples:

- Profiles
- TimeSeries
- Trajectories

Example:

```json
{
  "@type": "PropertyValue",
  "name": "DataType",
  "value": "Profiles"
}
```

---

## Conversion Hints

### primaryVariableTargetColumn

Defines the ODV primary variable.

For profile datasets this is typically required.

Example:

```json
{
  "name": "primaryVariableTargetColumn",
  "value": "Pressure (original) [decibar]"
}
```

### columnSeparator

Defines the source file separator.

Examples:

- tab (`\t`)
- comma
- semicolon

### fillValue

Defines the missing value marker.

---

## Column Mapping Model

Column mappings are represented through:

`Dataset.variableMeasured[]`

Each item represents one source column.

Required:

- `PropertyValue.name`

The value MUST match the original source column header.

The target ODV column is defined by:

`additionalProperty(name="targetColumn")`

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
```

---

## Optional Column Metadata

Columns MAY define:

### dataType

Allowed values:

- Text
- Number
- DateTime

### role

Allowed values:

- meta
- data

### P01

A NERC P01 vocabulary URI describing the parameter.

Example:

```json
{
  "name": "P01",
  "value": "https://vocab.nerc.ac.uk/collection/P01/current/ALONZZ01/"
}
```

---

## Processing Model

A converter SHOULD:

1. Validate JSON-LD against the ODIS2ODV JSON Schema
2. Read the source file from `distribution.contentUrl`
3. Apply dataset parsing hints
4. Extract source-to-target mappings from `variableMeasured`
5. Rename/reorder columns
6. Apply ODV-specific requirements
7. Emit an ODV Generic Spreadsheet

---

## Validation

Validation requires:

- valid JSON syntax
- JSON Schema Draft 2020-12 validation
- presence of required schema.org metadata
- deterministic column mappings

Recommended command:

```bash
./jsonValidate.bash
```

---

## Repository Components

- `schema/odv-odis2odv.schema.json`  
  Machine-readable JSON Schema

- `profile/odis-profile-odv-generic-spreadsheet.md`  
  Human-readable ODIS profile

- `examples/`  
  Valid example datasets

- `tools/`  
  Validation and conversion utilities

---

## Extensibility

Future extensions may support:

- quality flags
- multiple source files
- unit conversion
- additional source formats

Extensions MUST preserve existing semantics.

---

## Summary

This profile enables:

- ODIS-compatible dataset publication
- schema.org-only metadata
- reproducible ODV Generic Spreadsheet generation
- clear separation between discovery metadata, ODV metadata,
  and conversion rules
